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
    state->inputManager.Mouse.clearPointerLockHandler();
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
    return state->inputManager.Keyboard;
}

//------------------------------------------------------------------------------
const class Mouse&
Input::Mouse() {
    o_assert_dbg(IsValid());
    return state->inputManager.Mouse;
}

//------------------------------------------------------------------------------
const class Gamepad&
Input::Gamepad(int32 index) {
    o_assert_dbg(IsValid());
    return state->inputManager.Gamepad[index];
}

//------------------------------------------------------------------------------
const class Touchpad&
Input::Touchpad() {
    o_assert_dbg(IsValid());
    return state->inputManager.Touchpad;
}

//------------------------------------------------------------------------------
const class Sensors&
Input::Sensors() {
    o_assert_dbg(IsValid());
    return state->inputManager.Sensors;
}

//------------------------------------------------------------------------------
Keyboard::EventHandlerId
Input::SubscribeKeyboard(Keyboard::EventHandler handler) {
    o_assert_dbg(IsValid());
    return state->inputManager.Keyboard.subscribe(handler);
}

//------------------------------------------------------------------------------
void
Input::UnsubscribeKeyboard(Keyboard::EventHandlerId id) {
    o_assert_dbg(IsValid());
    state->inputManager.Keyboard.unsubscribe(id);
}

//------------------------------------------------------------------------------
Mouse::EventHandlerId
Input::SubscribeMouse(Mouse::EventHandler handler) {
    o_assert_dbg(IsValid());
    return state->inputManager.Mouse.subscribe(handler);
}

//------------------------------------------------------------------------------
void
Input::UnsubscribeMouse(Mouse::EventHandlerId id) {
    o_assert_dbg(IsValid());
    state->inputManager.Mouse.unsubscribe(id);
}

//------------------------------------------------------------------------------
Touchpad::EventHandlerId
Input::SubscribeTouchpad(Touchpad::EventHandler handler) {
    o_assert_dbg(IsValid());
    return state->inputManager.Touchpad.subscribe(handler);
}

//------------------------------------------------------------------------------
void
Input::UnsubscribeTouchpad(Touchpad::EventHandlerId id) {
    o_assert_dbg(IsValid());
    state->inputManager.Touchpad.unsubscribe(id);
}

//------------------------------------------------------------------------------
void
Input::SetMousePointerLockHandler(Mouse::PointerLockHandler handler) {
    o_assert_dbg(IsValid());
    state->inputManager.Mouse.setPointerLockHandler(handler);
}

//------------------------------------------------------------------------------
void
Input::ClearMousePointerLockHandler() {
    o_assert_dbg(IsValid());
    state->inputManager.Mouse.clearPointerLockHandler();
}

} // namespace Input
