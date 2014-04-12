//------------------------------------------------------------------------------
//  App.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "App.h"

namespace Oryol {
namespace Application {

//------------------------------------------------------------------------------
App::App() :
curState(AppState::Construct) {
    // empty
}

//------------------------------------------------------------------------------
App::~App() {
    // empty
}

//------------------------------------------------------------------------------
void
App::onFrame() {
    // if the current state is the destroy state, set
    if (AppState::Destroy == this->curState) {
        this->setQuitRequested();
    }
    switch (this->curState) {
        case AppState::Construct:
            this->curState = this->OnConstruct();
            break;
        case AppState::EnqueuePreload:
            this->curState = this->OnEnqueuePreload();
            break;
        case AppState::Preloading:
            this->curState = this->OnPreloading();
            break;
        case AppState::Init:
            this->curState = this->OnInit();
            break;
        case AppState::Running:
            this->curState = this->OnRunning();
            break;
        case AppState::Cleanup:
            this->curState = this->OnCleanup();
            break;
        case AppState::Destroy:
            this->curState = this->OnDestroy();
            break;
        default:
            // InvalidAppState is also set on end of OnDestroy
            AppBase::stopMainLoop();
            break;
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
    return AppState::Destroy;
}

//------------------------------------------------------------------------------
AppState::Code
App::OnDestroy() {
    return AppState::InvalidAppState;
}

} // namespace Application
} // namespace Oryol
