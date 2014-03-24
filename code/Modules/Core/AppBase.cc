//------------------------------------------------------------------------------
//  AppBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "AppBase.h"

namespace Oryol {
namespace Core {

AppBase* AppBase::self = nullptr;

//------------------------------------------------------------------------------
AppBase::AppBase() :
quitRequested(false) {
    self = this;
}

//------------------------------------------------------------------------------
AppBase::~AppBase() {
    self = nullptr;
}

//------------------------------------------------------------------------------
void
AppBase::StartMainLoop() {
    o_assert(nullptr != self);
    #if ORYOL_EMSCRIPTEN
    emscripten_set_main_loop(onFrame, 0, 1);
    #else
    while (!this->quitRequested) {
        onFrame();
    }
    #endif
}

//------------------------------------------------------------------------------
void
AppBase::onFrame() {
    o_assert(nullptr != self);
    if (self->frameCallback) {
        self->frameCallback();
    }
}

//------------------------------------------------------------------------------
void
AppBase::setFrameCallback(std::function<void(void)> cb) {
    this->frameCallback = cb;
}

//------------------------------------------------------------------------------
void
AppBase::setQuitRequested() {
    this->quitRequested = true;
}

//------------------------------------------------------------------------------
bool
AppBase::isQuitRequested() const {
    return this->quitRequested;
}

} // namespace Core
} // namespace Oryol