//------------------------------------------------------------------------------
//  App.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "App.h"
#include "Core/CoreFacade.h"
#if ORYOL_EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif
#if ORYOL_PNACL
#include "Core/pnacl/pnaclInstance.h"
#endif

namespace Oryol {
namespace Core {

App* App::self = nullptr;

//------------------------------------------------------------------------------
App::App() :
coreFacade(nullptr),
curState(AppState::Construct),
nextState(AppState::InvalidAppState),
quitRequested(false),
suspendRequested(false)
{
    self = this;
    #if ORYOL_ANDROID
    this->androidBridge.setup(this);
    #elif ORYOL_IOS
    this->iosBridge = iosBridge::CreateSingle(this);
    #endif
}

//------------------------------------------------------------------------------
App::~App() {
    #if ORYOL_ANDROID
    this->androidBridge.discard();
    #elif ORYOL_IOS
    iosBridge::DestroySingle();
    this->iosBridge = nullptr;
    #endif
    self = nullptr;
}

//------------------------------------------------------------------------------
void
App::StartMainLoop() {
    o_assert(nullptr != self);
    this->coreFacade = CoreFacade::CreateSingle();
    Log::Info("=> App::StartMainLoop()\n");
    #if ORYOL_EMSCRIPTEN
        emscripten_set_main_loop(staticOnFrame, 0, 1);
    #elif ORYOL_IOS
        this->iosBridge->startMainLoop();
    #elif ORYOL_ANDROID
        this->addBlocker(AppState::Init);
        this->androidBridge.onStart();
        while (this->androidBridge.onFrame()) {
        }
        this->androidBridge.onStop();
    #elif ORYOL_PNACL
        pnaclInstance::Instance()->startMainLoop(this);
    #else
        while (AppState::InvalidAppState != this->curState) {
            this->onFrame();
        }
    #endif

    // NOTE: PNaCl is the only platform where StartMainLoop
    // returns while the app continues running!
    #if !ORYOL_PNACL
    Log::Info("<= App::StartMainLoop()\n");
    CoreFacade::DestroySingle();
    this->coreFacade = nullptr;
    #endif
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
        this->blockers.Insert(blockedState);
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
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    else {
        // trigger central runloop
        this->coreFacade->RunLoop()->Run();

        // call current state handler function
        switch (this->curState) {
            case AppState::Construct:
                this->nextState = this->OnConstruct();
                break;
            case AppState::EnqueuePreload:
                this->nextState = this->OnEnqueuePreload();
                break;
            case AppState::Preloading:
                this->nextState = this->OnPreloading();
                break;
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
                Log::Warn("App::onFrame(): UNHANDLED APP STATE '%s'!\n", AppState::ToString(this->curState));
                break;
        }  
    }  
}

//------------------------------------------------------------------------------
AppState::Code
App::OnConstruct() {
    return AppState::EnqueuePreload;
}

//------------------------------------------------------------------------------
AppState::Code
App::OnEnqueuePreload() {
    return AppState::Preloading;
}

//------------------------------------------------------------------------------
AppState::Code
App::OnPreloading() {
    return AppState::Init;
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

} // namespace Core
} // namespace Oryol