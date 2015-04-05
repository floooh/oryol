//------------------------------------------------------------------------------
//  Input.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Memory/Memory.h"
#include "Input.h"

namespace Oryol {

struct Input::_state* Input::state = nullptr;
    
//------------------------------------------------------------------------------
void
Input::Setup(const InputSetup& setup) {
    o_assert_dbg(!IsValid());
    state = Memory::New<_state>();
    state->inputManager.setup(setup);
}

//------------------------------------------------------------------------------
void
Input::Discard() {
    o_assert_dbg(IsValid());
    state->inputManager.discard();
    Memory::Delete(state);
    state = nullptr;
}

//------------------------------------------------------------------------------
bool
Input::IsValid() {
    return nullptr != state;
}

//------------------------------------------------------------------------------
void
Input::AttachInputHandler(const Ptr<Port>& handler) {
    o_assert_dbg(IsValid());
    state->inputManager.attachInputHandler(handler);
}

//------------------------------------------------------------------------------
void
Input::DetachInputHandler(const Ptr<Port>& handler) {
    o_assert_dbg(IsValid());
    state->inputManager.detachInputHandler(handler);
}

//------------------------------------------------------------------------------
void
Input::SetCursorMode(CursorMode::Code mode) {
    o_assert_dbg(IsValid());
    state->inputManager.setCursorMode(mode);
}

//------------------------------------------------------------------------------
CursorMode::Code
Input::GetCursorMode() {
    o_assert_dbg(IsValid());
    return state->inputManager.getCursorMode();
}

//------------------------------------------------------------------------------
void
Input::BeginCaptureText() {
    o_assert_dbg(IsValid());
    state->inputManager.beginCaptureText();
}

//------------------------------------------------------------------------------
void
Input::EndCaptureText() {
    o_assert_dbg(IsValid());
    return state->inputManager.endCaptureText();
}

//------------------------------------------------------------------------------
const class Keyboard&
Input::Keyboard() {
    o_assert_dbg(IsValid());
    return state->inputManager.Keyboard();
}

//------------------------------------------------------------------------------
const class Mouse&
Input::Mouse() {
    o_assert_dbg(IsValid());
    return state->inputManager.Mouse();
}

//------------------------------------------------------------------------------
const class Gamepad&
Input::Gamepad(int32 index) {
    o_assert_dbg(IsValid());
    return state->inputManager.Gamepad(index);
}

//------------------------------------------------------------------------------
const class Touchpad&
Input::Touchpad() {
    o_assert_dbg(IsValid());
    return state->inputManager.Touchpad();
}

//------------------------------------------------------------------------------
const class Sensors&
Input::Sensors() {
    o_assert_dbg(IsValid());
    return state->inputManager.Sensors();
}

} // namespace Input
