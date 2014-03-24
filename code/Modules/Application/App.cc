//------------------------------------------------------------------------------
//  App.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "App.h"

namespace Oryol {
namespace Application {

using namespace std::placeholders;

//------------------------------------------------------------------------------
App::App() :
curState(AppState::Construct) {
    this->setFrameCallback(std::bind(&App::dispatch, this));
}

//------------------------------------------------------------------------------
App::~App() {
    // call the destruct state if set
    if (this->callbacks[AppState::Destroy]) {
        this->callbacks[AppState::Destroy]();
    }
}

//------------------------------------------------------------------------------
void
App::AddCallback(AppState::Code state, AppCallback cb) {
    o_assert(cb);
    o_assert(state < AppState::NumAppStates);
    this->callbacks[state] = cb;
}

//------------------------------------------------------------------------------
void
App::dispatch() {
    // if the current state is the destroy state, set
    if (AppState::Destroy == this->curState) {
        this->setQuitRequested();
    }
    // skip unset state callbacks
    while ((this->curState < AppState::NumAppStates) && !this->callbacks[this->curState]) {
        int stateNum = (int) this->curState;
        stateNum++;
        this->curState = (AppState::Code) stateNum;
    }
    // call state callback
    if (this->curState < AppState::NumAppStates) {
        o_assert(this->callbacks[this->curState]);
        this->curState = this->callbacks[this->curState]();
    }
    else {
        this->setQuitRequested();
    }
}

} // namespace Application
} // namespace Oryol
