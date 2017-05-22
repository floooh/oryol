//------------------------------------------------------------------------------
//  App.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "App.h"
#include "Core/Memory/Memory.h"
#include "Core/Core.h"
#include "Core/RunLoop.h"
#include "Core/Trace.h"
#if ORYOL_EMSCRIPTEN
#include <emscripten/emscripten.h>
#elif ORYOL_IOS
#include "Core/private/ios/iosBridge.h"
#elif ORYOL_MACOS && ORYOL_METAL
#include "Core/private/osx/osxBridge.h"
#elif ORYOL_ANDROID
#include "Core/private/android/androidBridge.h"
#endif
#include <chrono>
#include <thread>

namespace Oryol {

using namespace _priv;

App* App::self = nullptr;

//------------------------------------------------------------------------------
App::App() :
curState(AppState::Init),
nextState(AppState::InvalidAppState),
quitRequested(false),
suspendRequested(false)
{
    self = this;
    #if ORYOL_ANDROID
    this->androidBridge = Memory::New<_priv::androidBridge>();
    this->androidBridge->setup(this);
    #elif ORYOL_IOS
    this->iosBridge = Memory::New<_priv::iosBridge>();
    this->iosBridge->setup(this);
    #elif ORYOL_MACOS && ORYOL_METAL
    this->osxBridge = Memory::New<_priv::osxBridge>();
    this->osxBridge->setup(this);
    #endif
}

//------------------------------------------------------------------------------
App::~App() {
    #if ORYOL_MACOS && ORYOL_METAL
    this->osxBridge->discard();
    Memory::Delete(this->osxBridge);
    this->osxBridge = nullptr;
    #elif ORYOL_ANDROID
    this->androidBridge->discard();
    Memory::Delete(this->androidBridge);
    this->androidBridge = nullptr;
    #elif ORYOL_IOS
    this->iosBridge->discard();
    Memory::Delete(this->iosBridge);
    this->iosBridge = nullptr;
    #endif
    self = nullptr;
}

//------------------------------------------------------------------------------
void
App::StartMainLoop() {
    o_assert(nullptr != self);
    Core::Setup();
    Log::Info("=> App::StartMainLoop()\n");
    #if ORYOL_EMSCRIPTEN
        emscripten_set_main_loop(staticOnFrame, 0, 1);
    #elif ORYOL_IOS
        this->iosBridge->startMainLoop();
    #elif ORYOL_MACOS && ORYOL_METAL
        this->osxBridge->startMainLoop();
    #elif ORYOL_ANDROID
        this->addBlocker(AppState::Init);
        this->androidBridge->onStart();
        while (this->androidBridge->onFrame() && (AppState::InvalidAppState != this->curState)) {
            // empty
        }
        this->androidBridge->onStop();
    #else
        while (AppState::InvalidAppState != this->curState) {
            this->onFrame();
        }
    #endif
    Log::Info("<= App::StartMainLoop()\n");
    Core::Discard();
}

//------------------------------------------------------------------------------
void
App::staticOnFrame() {
    o_assert(nullptr != self);
    self->onFrame();
}

//------------------------------------------------------------------------------
void
App::addBlocker(AppState::Code blockedState) {
    if (!this->blockers.Contains(blockedState)) {
        this->blockers.Add(blockedState);
    }
}

//------------------------------------------------------------------------------
void
App::remBlocker(AppState::Code blockedState) {
    if (this->blockers.Contains(blockedState)) {
        this->blockers.Erase(blockedState);
    }
}

//------------------------------------------------------------------------------
void
App::onFrame() {
    o_trace_begin_frame();
    o_trace_scoped(App_OnFrame);
    
    // state transition?
    if ((this->nextState != AppState::InvalidAppState) && (this->nextState != this->curState)) {
        // check if the next state is blocked
        if (this->blockers.Contains(this->nextState)) {
            // yikes, we're blocked
            if (AppState::Blocked != this->curState) {
                Log::Info("App::onFrame(): state '%s' is blocked, switching to Blocked state!\n", AppState::ToString(this->nextState));
                this->curState = AppState::Blocked;
            }
        }
        else {
            // ok, not (or no longer) blocked
            Log::Info("App::onFrame(): Switching to app state '%s'\n", AppState::ToString(this->nextState));
            this->curState = this->nextState;
            this->nextState = AppState::InvalidAppState;
        }
    }

    // trigger the main loop
    if (AppState::Blocked == this->curState) {
        // we're currently blocked by some external force
        o_trace_scoped(App_Blocked);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    else {
        // trigger the 'before-frame' runloop
        o_trace_begin(App_PreRunLoop);
        Core::PreRunLoop()->Run();
        o_trace_end();
    
        // call current state handler function
        o_trace_begin(App_InnerFrame);
        switch (this->curState) {
            case AppState::Init:
                this->nextState = this->OnInit();
                break;
            case AppState::Running:
                this->nextState = this->OnRunning();
                break;
            case AppState::Cleanup:
                this->nextState = this->OnCleanup();
                break;
            case AppState::Destroy:
                this->nextState = this->OnDestroy();
                this->curState = AppState::InvalidAppState;
                break;
            default:
                o_warn("App::onFrame(): UNHANDLED APP STATE '%s'!\n", AppState::ToString(this->curState));
                break;
        }
        o_trace_end();

        // trigger the 'after-frame' runloop
        o_trace_begin(App_PostRunLoop);
        Core::PostRunLoop()->Run();
        o_trace_end();
    }
    o_trace_end_frame();
}

//------------------------------------------------------------------------------
AppState::Code
App::OnInit() {
    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
App::OnRunning() {
    return AppState::Cleanup;
}

//------------------------------------------------------------------------------
AppState::Code
App::OnCleanup() {
    // NOTE: on mobile apps, Cleanup will go back into the Init state
    // (which will be blocked until the app is resumed), and the OS ultimately
    // decides when to actually kill the application
    // on Desktop platforms, Cleanup will always go to Destroy
    // I don't know yet whether this is the best way to do it, ultimately
    // we want a proper START -> RESUME -> RUNNING -> SUSPEND -> STOP
    // cycle, where SUSPEND can either go to RESUME or to STOP
    if (this->quitRequested) {
        return AppState::Destroy;
    }
    else if (this->suspendRequested) {
        this->addBlocker(AppState::Init);
        return AppState::Init;
    }
    else {
        return AppState::Destroy;
    }
}

//------------------------------------------------------------------------------
AppState::Code
App::OnDestroy() {
    #if ORYOL_EMSCRIPTEN
    emscripten_cancel_main_loop();
    #endif
    #if ORYOL_MACOS && ORYOL_METAL
    this->osxBridge->onDestroy();
    #endif
    return AppState::InvalidAppState;
}

//------------------------------------------------------------------------------
void
App::readyForInit() {
    this->remBlocker(AppState::Init);
}

//------------------------------------------------------------------------------
void
App::requestQuit() {
    if (AppState::Running == this->curState) {
        this->nextState = AppState::Cleanup;
    }
    else {
        this->nextState = AppState::Destroy;
    }
    this->quitRequested = true;
}

//------------------------------------------------------------------------------
void
App::requestSuspend() {
    this->nextState = AppState::Cleanup;
    this->suspendRequested = true;
}

} // namespace Oryol
