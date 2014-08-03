//------------------------------------------------------------------------------
//  InputFacade.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "InputFacade.h"

namespace Oryol {
namespace Input {
    
OryolLocalSingletonImpl(InputFacade);

//------------------------------------------------------------------------------
InputFacade::InputFacade() {
    this->SingletonEnsureUnique();
}

//------------------------------------------------------------------------------
InputFacade::~InputFacade() {
    // empty
}

//------------------------------------------------------------------------------
void
InputFacade::SetCursorMode(CursorMode::Code mode) {
    this->inputManager.setCursorMode(mode);
}

//------------------------------------------------------------------------------
CursorMode::Code
InputFacade::GetCursorMode() const {
    return this->inputManager.getCursorMode();
}

//------------------------------------------------------------------------------
void
InputFacade::BeginCaptureText() {
    this->inputManager.beginCaptureText();
}

//------------------------------------------------------------------------------
void
InputFacade::EndCaptureText() {
    return this->inputManager.endCaptureText();
}

//------------------------------------------------------------------------------
const class Keyboard&
InputFacade::Keyboard() const {
    return this->inputManager.Keyboard();
}

//------------------------------------------------------------------------------
const class Mouse&
InputFacade::Mouse() const {
    return this->inputManager.Mouse();
}

//------------------------------------------------------------------------------
const class Gamepad&
InputFacade::Gamepad(int32 index) const {
    return this->inputManager.Gamepad(index);
}

//------------------------------------------------------------------------------
const class Touchpad&
InputFacade::Touchpad() const {
    return this->inputManager.Touchpad();
}

} // namespace Input
} // namespace Oryol