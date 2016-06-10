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
    state->inputManager.dispatcher.pointerLockHandler = 0;
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
Input::CallbackId
Input::SubscribeInputEvents(InputEventCallback cb) {
    o_assert_dbg(state);
    return state->inputManager.dispatcher.subscribeEvents(cb);
}

//------------------------------------------------------------------------------
void
Input::UnsubscribeInputEvents(CallbackId id) {
    o_assert_dbg(state);
    state->inputManager.dispatcher.unsubscribeEvents(id);
}

//------------------------------------------------------------------------------
void
Input::SetMousePointerLockHandler(PointerLockCallback cb) {
    o_assert_dbg(state);
    state->inputManager.dispatcher.pointerLockHandler = cb;
}

//------------------------------------------------------------------------------
void
Input::ClearMousePointerLockHandler() {
    o_assert_dbg(state);
    state->inputManager.dispatcher.pointerLockHandler = 0;
}

//------------------------------------------------------------------------------
bool
Input::KeyboardAttached() {
    o_assert_dbg(state);
    return state->inputManager.keyboard.attached;
}

//------------------------------------------------------------------------------
bool
Input::KeyPressed(Key::Code key) {
    o_assert_dbg(state);
    return state->inputManager.keyboard.keyPressed(key);
}

//------------------------------------------------------------------------------
bool
Input::KeyDown(Key::Code key) {
    o_assert_dbg(state);
    return state->inputManager.keyboard.keyDown(key);
}

//------------------------------------------------------------------------------
bool
Input::KeyUp(Key::Code key) {
    o_assert_dbg(state);
    return state->inputManager.keyboard.keyUp(key);
}

//------------------------------------------------------------------------------
bool
Input::KeyRepeat(Key::Code key) {
    o_assert_dbg(state);
    return state->inputManager.keyboard.keyRepeat(key);
}

//------------------------------------------------------------------------------
bool
Input::AnyKeyPressed() {
    o_assert_dbg(state);
    return state->inputManager.keyboard.anyKeyPressed();
}

//------------------------------------------------------------------------------
bool
Input::AnyKeyDown() {
    o_assert_dbg(state);
    return state->inputManager.keyboard.anyKeyDown();
}

//------------------------------------------------------------------------------
bool
Input::AnyKeyUp() {
    o_assert_dbg(state);
    return state->inputManager.keyboard.anyKeyUp();
}

//------------------------------------------------------------------------------
bool
Input::AnyKeyRepeat() {
    o_assert_dbg(state);
    return state->inputManager.keyboard.anyKeyRepeat();
}

//------------------------------------------------------------------------------
const wchar_t*
Input::CapturedText() {
    o_assert_dbg(state);
    return state->inputManager.keyboard.capturedText();
}

//------------------------------------------------------------------------------
bool
Input::MouseAttached() {
    o_assert_dbg(state);
    return state->inputManager.mouse.attached;
}

//------------------------------------------------------------------------------
bool
Input::MouseButtonPressed(MouseButton::Code btn) {
    o_assert_dbg(state);
    return state->inputManager.mouse.buttonPressed(btn);
}

//------------------------------------------------------------------------------
bool
Input::MouseButtonDown(MouseButton::Code btn) {
    o_assert_dbg(state);
    return state->inputManager.mouse.buttonDown(btn);
}

//------------------------------------------------------------------------------
bool
Input::MouseButtonUp(MouseButton::Code btn) {
    o_assert_dbg(state);
    return state->inputManager.mouse.buttonUp(btn);
}

//------------------------------------------------------------------------------
const glm::vec2&
Input::MousePosition() {
    o_assert_dbg(state);
    return state->inputManager.mouse.position;
}

//------------------------------------------------------------------------------
const glm::vec2&
Input::MouseMovement() {
    o_assert_dbg(state);
    return state->inputManager.mouse.movement;
}

//------------------------------------------------------------------------------
const glm::vec2&
Input::MouseScroll() {
    o_assert_dbg(state);
    return state->inputManager.mouse.scroll;
}

//------------------------------------------------------------------------------
bool
Input::GamepadAttached(int gamepadIndex) {
    o_assert_dbg(state);
    return state->inputManager.gamepad[gamepadIndex].attached;
}

//------------------------------------------------------------------------------
bool
Input::GamepadButtonPressed(int gamepadIndex, GamepadGizmo::Code btn) {
    o_assert_dbg(state);
    return state->inputManager.gamepad[gamepadIndex].buttonPressed(btn);
}

//------------------------------------------------------------------------------
bool
Input::GamepadButtonDown(int gamepadIndex, GamepadGizmo::Code btn) {
    o_assert_dbg(state);
    return state->inputManager.gamepad[gamepadIndex].buttonDown(btn);
}

//------------------------------------------------------------------------------
bool
Input::GamepadButtonUp(int gamepadIndex, GamepadGizmo::Code btn) {
    o_assert_dbg(state);
    return state->inputManager.gamepad[gamepadIndex].buttonUp(btn);
}

//------------------------------------------------------------------------------
float
Input::GamepadTriggerValue(int gamepadIndex, GamepadGizmo::Code trigger) {
    o_assert_dbg(state);
    return state->inputManager.gamepad[gamepadIndex].triggerValue(trigger);
}

//------------------------------------------------------------------------------
const glm::vec2&
Input::GamepadStickPos(int gamepadIndex, GamepadGizmo::Code stick) {
    o_assert_dbg(state);
    return state->inputManager.gamepad[gamepadIndex].stickPos(stick);
}

//------------------------------------------------------------------------------
bool
Input::TouchpadAttached() {
    o_assert_dbg(state);
    return state->inputManager.touchpad.attached;
}

//------------------------------------------------------------------------------
bool
Input::TouchTapped() {
    o_assert_dbg(state);
    return state->inputManager.touchpad.tapped;
}

//------------------------------------------------------------------------------
bool
Input::TouchDoubleTapped() {
    o_assert_dbg(state);
    return state->inputManager.touchpad.doubleTapped;
}

//------------------------------------------------------------------------------
bool
Input::TouchPanningStarted() {
    o_assert_dbg(state);
    return state->inputManager.touchpad.panningStarted;
}

//------------------------------------------------------------------------------
bool
Input::TouchPanning() {
    o_assert_dbg(state);
    return state->inputManager.touchpad.panning;
}

//------------------------------------------------------------------------------
bool
Input::TouchPanningEnded() {
    o_assert_dbg(state);
    return state->inputManager.touchpad.panningEnded;
}

//------------------------------------------------------------------------------
bool
Input::TouchPinchingStarted() {
    o_assert_dbg(state);
    return state->inputManager.touchpad.pinchingStarted;
}

//------------------------------------------------------------------------------
bool
Input::TouchPinching() {
    o_assert_dbg(state);
    return state->inputManager.touchpad.pinching;
}

//------------------------------------------------------------------------------
bool
Input::TouchPinchingEnded() {
    o_assert_dbg(state);
    return state->inputManager.touchpad.pinchingEnded;
}

//------------------------------------------------------------------------------
const glm::vec2&
Input::TouchPosition(int touchIndex) {
    o_assert_dbg(state);
    return state->inputManager.touchpad.position[touchIndex];
}

//------------------------------------------------------------------------------
const glm::vec2&
Input::TouchMovement(int touchIndex) {
    o_assert_dbg(state);
    return state->inputManager.touchpad.movement[touchIndex];
}

//------------------------------------------------------------------------------
const glm::vec2&
Input::TouchStartPosition(int touchIndex) {
    o_assert_dbg(state);
    return state->inputManager.touchpad.startPosition[touchIndex];
}

//------------------------------------------------------------------------------
bool
Input::SensorsAttached() {
    o_assert_dbg(state);
    return state->inputManager.sensors.attached;
}

//------------------------------------------------------------------------------
const glm::vec3&
Input::SensorAcceleration() {
    o_assert_dbg(state);
    return state->inputManager.sensors.acceleration;
}

//------------------------------------------------------------------------------
float
Input::SensorYaw() {
    o_assert_dbg(state);
    return state->inputManager.sensors.yaw;
}

//------------------------------------------------------------------------------
float
Input::SensorPitch() {
    o_assert_dbg(state);
    return state->inputManager.sensors.pitch;
}

//------------------------------------------------------------------------------
float
Input::SensorRoll() {
    o_assert_dbg(state);
    return state->inputManager.sensors.roll;
}

} // namespace Input
