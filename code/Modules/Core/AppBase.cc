//------------------------------------------------------------------------------
//  AppBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "AppBase.h"
#include "Core/CoreFacade.h"
#if ORYOL_EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

namespace Oryol {
namespace Core {

AppBase* AppBase::self = nullptr;

//------------------------------------------------------------------------------
AppBase::AppBase() :
coreFacade(nullptr),
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
    this->coreFacade = CoreFacade::CreateSingleton();
    #if ORYOL_EMSCRIPTEN
    emscripten_set_main_loop(staticOnFrame, 0, 1);
    #else
    while (!this->quitRequested) {
        onFrame();
    }
    #endif
    CoreFacade::DestroySingleton();
    this->coreFacade = nullptr;
}

//------------------------------------------------------------------------------
void
AppBase::stopMainLoop() {
    #if ORYOL_EMSCRIPTEN
    emscripten_cancel_main_loop();
    #endif
    this->quitRequested = true;
}

//------------------------------------------------------------------------------
void
AppBase::staticOnFrame() {
    o_assert(nullptr != self);
    self->onFrame();
}

//------------------------------------------------------------------------------
void
AppBase::onFrame() {
    if (nullptr != this->coreFacade) {
        this->coreFacade->RunLoop()->Run();
    }
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