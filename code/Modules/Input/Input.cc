//------------------------------------------------------------------------------
//  Input.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Memory/Memory.h"
#include "Input.h"
#include "Input/private/inputMgr.h"

namespace Oryol {

struct _input_state {
    _priv::inputMgr inputManager;
};
_input_state* inputState = nullptr;
    
//------------------------------------------------------------------------------
void
Input::Setup(const InputSetup& setup) {
    o_assert_dbg(!IsValid());
    inputState = Memory::New<_input_state>();
    inputState->inputManager.setup(setup);
}

//------------------------------------------------------------------------------
void
Input::Discard() {
    o_assert_dbg(IsValid());
    inputState->inputManager.dispatcher.pointerLockHandler = nullptr;
    inputState->inputManager.discard();
    Memory::Delete(inputState);
    inputState = nullptr;
}

//------------------------------------------------------------------------------
bool
Input::IsValid() {
    return nullptr != inputState;
}

//------------------------------------------------------------------------------
Input::CallbackId
Input::SubscribeEvents(InputEventCallback cb) {
    o_assert_dbg(inputState);
    return inputState->inputManager.dispatcher.subscribeEvents(cb);
}

//------------------------------------------------------------------------------
void
Input::UnsubscribeEvents(CallbackId id) {
    o_assert_dbg(inputState);
    inputState->inputManager.dispatcher.unsubscribeEvents(id);
}

//------------------------------------------------------------------------------
void
Input::SetPointerLockHandler(PointerLockCallback cb) {
    o_assert_dbg(inputState);
    inputState->inputManager.dispatcher.pointerLockHandler = cb;
}

//------------------------------------------------------------------------------
void
Input::ClearPointerLockHandler() {
    o_assert_dbg(inputState);
    inputState->inputManager.dispatcher.pointerLockHandler = nullptr;
}

//------------------------------------------------------------------------------
bool
Input::KeyboardAttached() {
    o_assert_dbg(inputState);
    return inputState->inputManager.keyboard.attached;
}

//------------------------------------------------------------------------------
bool
Input::KeyPressed(Key::Code key) {
    o_assert_dbg(inputState);
    return inputState->inputManager.keyboard.keyPressed(key);
}

//------------------------------------------------------------------------------
bool
Input::KeyDown(Key::Code key) {
    o_assert_dbg(inputState);
    return inputState->inputManager.keyboard.keyDown(key);
}

//------------------------------------------------------------------------------
bool
Input::KeyUp(Key::Code key) {
    o_assert_dbg(inputState);
    return inputState->inputManager.keyboard.keyUp(key);
}

//------------------------------------------------------------------------------
bool
Input::KeyRepeat(Key::Code key) {
    o_assert_dbg(inputState);
    return inputState->inputManager.keyboard.keyRepeat(key);
}

//------------------------------------------------------------------------------
bool
Input::AnyKeyPressed() {
    o_assert_dbg(inputState);
    return inputState->inputManager.keyboard.anyKeyPressed();
}

//------------------------------------------------------------------------------
bool
Input::AnyKeyDown() {
    o_assert_dbg(inputState);
    return inputState->inputManager.keyboard.anyKeyDown();
}

//------------------------------------------------------------------------------
bool
Input::AnyKeyUp() {
    o_assert_dbg(inputState);
    return inputState->inputManager.keyboard.anyKeyUp();
}

//------------------------------------------------------------------------------
bool
Input::AnyKeyRepeat() {
    o_assert_dbg(inputState);
    return inputState->inputManager.keyboard.anyKeyRepeat();
}

//------------------------------------------------------------------------------
const wchar_t*
Input::Text() {
    o_assert_dbg(inputState);
    return inputState->inputManager.keyboard.capturedText();
}

//------------------------------------------------------------------------------
bool
Input::MouseAttached() {
    o_assert_dbg(inputState);
    return inputState->inputManager.mouse.attached;
}

//------------------------------------------------------------------------------
bool
Input::MouseButtonPressed(MouseButton::Code btn) {
    o_assert_dbg(inputState);
    return inputState->inputManager.mouse.buttonPressed(btn);
}

//------------------------------------------------------------------------------
bool
Input::MouseButtonDown(MouseButton::Code btn) {
    o_assert_dbg(inputState);
    return inputState->inputManager.mouse.buttonDown(btn);
}

//------------------------------------------------------------------------------
bool
Input::MouseButtonUp(MouseButton::Code btn) {
    o_assert_dbg(inputState);
    return inputState->inputManager.mouse.buttonUp(btn);
}

//------------------------------------------------------------------------------
const glm::vec2&
Input::MousePosition() {
    o_assert_dbg(inputState);
    return inputState->inputManager.mouse.position;
}

//------------------------------------------------------------------------------
const glm::vec2&
Input::MouseMovement() {
    o_assert_dbg(inputState);
    return inputState->inputManager.mouse.movement;
}

//------------------------------------------------------------------------------
const glm::vec2&
Input::MouseScroll() {
    o_assert_dbg(inputState);
    return inputState->inputManager.mouse.scroll;
}

//------------------------------------------------------------------------------
void
Input::AddGamepadMapping(const StringAtom& gamepadTypeId, const GamepadMapping& mapping) {
    o_assert_dbg(inputState);
    inputState->inputManager.addGamepadMapping(gamepadTypeId, mapping);
}

//------------------------------------------------------------------------------
bool
Input::GamepadAttached(int gamepadIndex) {
    o_assert_dbg(inputState);
    return inputState->inputManager.gamepad[gamepadIndex].attached;
}

//------------------------------------------------------------------------------
StringAtom
Input::GamepadTypeId(int gamepadIndex) {
    o_assert_dbg(inputState);
    return inputState->inputManager.gamepad[gamepadIndex].id;
}

//------------------------------------------------------------------------------
bool
Input::GamepadRawButtonPressed(int gamepadIndex, int btnIndex) {
    o_assert_dbg(inputState);
    return inputState->inputManager.gamepad[gamepadIndex].buttonPressed(btnIndex);
}

//------------------------------------------------------------------------------
bool
Input::GamepadRawButtonDown(int gamepadIndex, int btnIndex) {
    o_assert_dbg(inputState);
    return inputState->inputManager.gamepad[gamepadIndex].buttonDown(btnIndex);
}

//------------------------------------------------------------------------------
bool
Input::GamepadRawButtonUp(int gamepadIndex, int btnIndex) {
    o_assert_dbg(inputState);
    return inputState->inputManager.gamepad[gamepadIndex].buttonUp(btnIndex);
}

//------------------------------------------------------------------------------
float
Input::GamepadRawAxisValue(int gamepadIndex, int axisIndex) {
    o_assert_dbg(inputState);
    return inputState->inputManager.gamepad[gamepadIndex].axisValue(axisIndex);
}

//------------------------------------------------------------------------------
bool
Input::GamepadButtonPressed(int gamepadIndex, GamepadButton::Code btn) {
    o_assert_dbg(inputState);
    o_assert_range_dbg(btn, GamepadButton::NumButtons);
    const auto& pad = inputState->inputManager.gamepad[gamepadIndex];
    int btnIndex = pad.mapping.Buttons[btn];
    if (InvalidIndex != btnIndex) {
        return pad.buttonPressed(btnIndex);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool
Input::GamepadButtonDown(int gamepadIndex, GamepadButton::Code btn) {
    o_assert_dbg(inputState);
    o_assert_range_dbg(btn, GamepadButton::NumButtons);
    const auto& pad = inputState->inputManager.gamepad[gamepadIndex];
    int btnIndex = pad.mapping.Buttons[btn];
    if (InvalidIndex != btnIndex) {
        return pad.buttonDown(btnIndex);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool
Input::GamepadButtonUp(int gamepadIndex, GamepadButton::Code btn) {
    o_assert_dbg(inputState);
    o_assert_range_dbg(btn, GamepadButton::NumButtons);
    const auto& pad = inputState->inputManager.gamepad[gamepadIndex];
    int btnIndex = pad.mapping.Buttons[btn];
    if (InvalidIndex != btnIndex) {
        return pad.buttonUp(btnIndex);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
float
Input::GamepadAxisValue(int gamepadIndex, GamepadAxis::Code axis) {
    o_assert_dbg(inputState);
    o_assert_range_dbg(axis, GamepadAxis::NumAxes);
    const auto& pad = inputState->inputManager.gamepad[gamepadIndex];
    const auto& axisMapping = pad.mapping.Axes[axis];
    if (InvalidIndex != axisMapping.Axis) {
        float val = pad.axisValue(axisMapping.Axis);
        val = (val * axisMapping.Scale) + axisMapping.Bias;
        // FIXME: deadzone        
        return val;
    }
    else {
        return 0.0f;
    }
}

//------------------------------------------------------------------------------
bool
Input::TouchpadAttached() {
    o_assert_dbg(inputState);
    return inputState->inputManager.touchpad.attached;
}

//------------------------------------------------------------------------------
bool
Input::TouchTouched() {
    o_assert_dbg(inputState);
    return inputState->inputManager.touchpad.touched;
}

//------------------------------------------------------------------------------
bool
Input::TouchTapped() {
    o_assert_dbg(inputState);
    return inputState->inputManager.touchpad.tapped;
}

//------------------------------------------------------------------------------
bool
Input::TouchDoubleTapped() {
    o_assert_dbg(inputState);
    return inputState->inputManager.touchpad.doubleTapped;
}

//------------------------------------------------------------------------------
bool
Input::TouchPanningStarted() {
    o_assert_dbg(inputState);
    return inputState->inputManager.touchpad.panningStarted;
}

//------------------------------------------------------------------------------
bool
Input::TouchPanning() {
    o_assert_dbg(inputState);
    return inputState->inputManager.touchpad.panning;
}

//------------------------------------------------------------------------------
bool
Input::TouchPanningEnded() {
    o_assert_dbg(inputState);
    return inputState->inputManager.touchpad.panningEnded;
}

//------------------------------------------------------------------------------
bool
Input::TouchPinchingStarted() {
    o_assert_dbg(inputState);
    return inputState->inputManager.touchpad.pinchingStarted;
}

//------------------------------------------------------------------------------
bool
Input::TouchPinching() {
    o_assert_dbg(inputState);
    return inputState->inputManager.touchpad.pinching;
}

//------------------------------------------------------------------------------
bool
Input::TouchPinchingEnded() {
    o_assert_dbg(inputState);
    return inputState->inputManager.touchpad.pinchingEnded;
}

//------------------------------------------------------------------------------
const glm::vec2&
Input::TouchPosition(int touchIndex) {
    o_assert_dbg(inputState);
    return inputState->inputManager.touchpad.position[touchIndex];
}

//------------------------------------------------------------------------------
const glm::vec2&
Input::TouchMovement(int touchIndex) {
    o_assert_dbg(inputState);
    return inputState->inputManager.touchpad.movement[touchIndex];
}

//------------------------------------------------------------------------------
const glm::vec2&
Input::TouchStartPosition(int touchIndex) {
    o_assert_dbg(inputState);
    return inputState->inputManager.touchpad.startPosition[touchIndex];
}

//------------------------------------------------------------------------------
bool
Input::SensorsAttached() {
    o_assert_dbg(inputState);
    return inputState->inputManager.sensors.attached;
}

//------------------------------------------------------------------------------
const glm::vec3&
Input::SensorAcceleration() {
    o_assert_dbg(inputState);
    return inputState->inputManager.sensors.acceleration;
}

//------------------------------------------------------------------------------
const glm::vec3&
Input::SensorYawPitchRoll() {
    o_assert_dbg(inputState);
    return inputState->inputManager.sensors.yawPitchRoll;
}

} // namespace Input
