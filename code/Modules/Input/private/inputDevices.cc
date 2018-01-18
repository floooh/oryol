//------------------------------------------------------------------------------
//  inputDevices.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "inputDevices.h"
#include "inputDispatcher.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
keyboardDevice::keyboardDevice() {
    this->chars.Fill(0);
}
//------------------------------------------------------------------------------
void
keyboardDevice::onKeyDown(Key::Code key) {
    o_assert_range_dbg(key, Key::NumKeys);
    o_assert_dbg(this->dispatcher);

    this->down[key] = true;
    this->pressed[key] = true;
    this->dispatcher->notifyEvent(InputEvent(InputEvent::KeyDown, key));
}

//------------------------------------------------------------------------------
void
keyboardDevice::onKeyUp(Key::Code key) {
    o_assert_range_dbg(key, Key::NumKeys);
    o_assert_dbg(this->dispatcher);

    this->up[key] = true;
    this->pressed[key] = false;
    this->dispatcher->notifyEvent(InputEvent(InputEvent::KeyUp, key));
}

//------------------------------------------------------------------------------
void
keyboardDevice::onKeyRepeat(Key::Code key) {
    o_assert_range_dbg(key, Key::NumKeys);
    o_assert_dbg(this->dispatcher);

    this->repeat[key] = true;
    this->dispatcher->notifyEvent(InputEvent(InputEvent::KeyRepeat, key));
}

//------------------------------------------------------------------------------
void
keyboardDevice::onChar(wchar_t c) {
    o_assert_dbg(this->dispatcher);

    if (this->charIndex < MaxNumChars) {
        this->chars[this->charIndex++] = c;
        this->chars[this->charIndex] = 0;
    }
    this->dispatcher->notifyEvent(InputEvent(InputEvent::WChar, c));
}

//------------------------------------------------------------------------------
void
keyboardDevice::clearCapturedText() {
    this->chars[0] = 0;
    this->charIndex = 0;
}

//------------------------------------------------------------------------------
void
keyboardDevice::reset() {
    this->down.reset();
    this->up.reset();
    this->repeat.reset();
    this->clearCapturedText();
}

//------------------------------------------------------------------------------
bool
keyboardDevice::keyPressed(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->pressed[key];
}

//------------------------------------------------------------------------------
bool
keyboardDevice::keyDown(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->down[key];
}

//------------------------------------------------------------------------------
bool
keyboardDevice::keyUp(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->up[key];
}

//------------------------------------------------------------------------------
bool
keyboardDevice::keyRepeat(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->repeat[key];
}

//------------------------------------------------------------------------------
bool
keyboardDevice::anyKeyPressed() const {
    return this->pressed.any();
}

//------------------------------------------------------------------------------
bool
keyboardDevice::anyKeyDown() const {
    return this->down.any();
}

//------------------------------------------------------------------------------
bool
keyboardDevice::anyKeyUp() const {
    return this->up.any();
}

//------------------------------------------------------------------------------
bool
keyboardDevice::anyKeyRepeat() const {
    return this->repeat.any();
}

//------------------------------------------------------------------------------
const wchar_t*
keyboardDevice::capturedText() const {
    return this->chars.begin();
}

//------------------------------------------------------------------------------
mouseDevice::mouseDevice() {
    this->buttonState.Fill(0);
}

//------------------------------------------------------------------------------
PointerLockMode::Code
mouseDevice::onButtonDown(MouseButton::Code btn) {
    o_assert_dbg(this->dispatcher);

    this->buttonState[btn] |= (btnDown | btnPressed);
    InputEvent ie(InputEvent::MouseButtonDown, btn);
    this->dispatcher->notifyEvent(ie);
    return this->dispatcher->notifyPointerLock(ie);
}

//------------------------------------------------------------------------------
PointerLockMode::Code
mouseDevice::onButtonUp(MouseButton::Code btn) {
    o_assert_dbg(this->dispatcher);

    this->buttonState[btn] &= ~btnPressed;
    this->buttonState[btn] |= btnUp;
    InputEvent ie(InputEvent::MouseButtonUp, btn);
    this->dispatcher->notifyEvent(ie);
    return this->dispatcher->notifyPointerLock(ie);
}

//------------------------------------------------------------------------------
void
mouseDevice::onPosMov(const glm::vec2& pos) {
    o_assert_dbg(this->dispatcher);

    this->movement = pos - this->position;
    this->position = pos;
    this->dispatcher->notifyEvent(InputEvent(InputEvent::MouseMove, this->movement, this->position));
}

//------------------------------------------------------------------------------
void
mouseDevice::onPosMov(const glm::vec2& pos, const glm::vec2& mov) {
    o_assert_dbg(this->dispatcher);

    this->movement = mov;
    this->position = pos;
    this->dispatcher->notifyEvent(InputEvent(InputEvent::MouseMove, this->movement, this->position));
}

//------------------------------------------------------------------------------
void
mouseDevice::onMov(const glm::vec2& mov) {
    o_assert_dbg(this->dispatcher);

    this->movement = mov;
    this->dispatcher->notifyEvent(InputEvent(InputEvent::MouseMove, this->movement, this->position));
}

//------------------------------------------------------------------------------
void
mouseDevice::onScroll(const glm::vec2& scroll) {
    o_assert_dbg(this->dispatcher);

    this->scroll = scroll;
    this->dispatcher->notifyEvent(InputEvent(InputEvent::MouseScrolling, this->scroll));
}

//------------------------------------------------------------------------------
void
mouseDevice::reset() {
    for (int i = 0; i < MouseButton::NumMouseButtons; i++) {
        this->buttonState[i] &= ~(btnDown | btnUp);
    }
    this->movement = glm::vec2(0.0f, 0.0f);
    this->scroll = glm::vec2(0.0f, 0.0f);
}

//------------------------------------------------------------------------------
bool
mouseDevice::buttonPressed(MouseButton::Code btn) const {
    o_assert_range_dbg(btn, MouseButton::NumMouseButtons);
    return 0 != (this->buttonState[btn] & btnPressed);
}

//------------------------------------------------------------------------------
bool
mouseDevice::buttonDown(MouseButton::Code btn) const {
    o_assert_range_dbg(btn, MouseButton::NumMouseButtons);
    return 0 != (this->buttonState[btn] & btnDown);
}

//------------------------------------------------------------------------------
bool
mouseDevice::buttonUp(MouseButton::Code btn) const {
    o_assert_range_dbg(btn, MouseButton::NumMouseButtons);
    return 0 != (this->buttonState[btn] & btnUp);
}

//------------------------------------------------------------------------------
void
touchpadDevice::reset() {
    this->touchStarted = false;
    this->touchMoved = false;
    this->touchEnded = false;
    this->touchCancelled = false;
    this->tapped = false;
    this->doubleTapped = false;
    this->panningStarted = false;
    this->panningEnded = false;
    this->pinchingStarted = false;
    this->pinchingEnded = false;
    for (int i = 0; i < MaxNumTouches; i++) {
        this->movement[i] = glm::vec2(0.0f, 0.0f);
    }
}

//------------------------------------------------------------------------------
void
touchpadDevice::onTouchEvent(touchEvent::touchType type, const glm::vec2& p) {
    o_assert_dbg(this->dispatcher);
    switch (type) {
        case touchEvent::began:
            this->touchStarted = true;
            break;
        case touchEvent::moved:
            this->touchMoved = true;
            break;
        case touchEvent::ended:
            this->touchEnded = true;
            break;
        case touchEvent::cancelled:
            this->touchCancelled = true;
            break;
        default:
            break;
    }
    this->position[0] = p;
}

//------------------------------------------------------------------------------
void
touchpadDevice::onTapped(const glm::vec2& p) {
    o_assert_dbg(this->dispatcher);

    this->tapped = true;
    this->position[0] = p;
    this->startPosition[0] = p;

    InputEvent ie;
    ie.Type = InputEvent::TouchTapped;
    ie.TouchPosition[0] = this->position[0];
    ie.TouchStartPosition[0] = this->startPosition[0];
    this->dispatcher->notifyEvent(ie);
}

//------------------------------------------------------------------------------
void
touchpadDevice::onDoubleTapped(const glm::vec2& p) {
    o_assert_dbg(this->dispatcher);

    this->doubleTapped = true;
    this->position[0] = p;
    this->startPosition[0] = p;

    InputEvent ie;
    ie.Type = InputEvent::TouchDoubleTapped;
    ie.TouchPosition[0] = this->position[0];
    ie.TouchStartPosition[0] = this->startPosition[0];
    this->dispatcher->notifyEvent(ie);
}

//------------------------------------------------------------------------------
void
touchpadDevice::onPanningStarted(const glm::vec2& p, const glm::vec2& start) {
    o_assert_dbg(this->dispatcher);

    this->panningStarted = true;
    this->panning = true;
    this->position[0] = p;
    this->startPosition[0] = start;

    InputEvent ie;
    ie.Type = InputEvent::TouchPanningStarted;
    ie.TouchPosition[0] = this->position[0];
    ie.TouchStartPosition[0] = this->startPosition[0];
    this->dispatcher->notifyEvent(ie);
}

//------------------------------------------------------------------------------
void
touchpadDevice::onPanning(const glm::vec2& p, const glm::vec2& start) {
    o_assert_dbg(this->dispatcher);

    // FIXME: hmm on iOS movement is always 0 because touch events
    // fire with the same value :/
    this->panning = true;
    this->movement[0] = p - this->position[0];
    this->position[0] = p;
    this->startPosition[0] = start;
    InputEvent ie;
    ie.Type = InputEvent::TouchPanning;
    ie.TouchPosition[0] = this->position[0];
    ie.TouchMovement[0] = this->movement[0];
    ie.TouchStartPosition[0] = this->startPosition[0];
    this->dispatcher->notifyEvent(ie);
}

//------------------------------------------------------------------------------
void
touchpadDevice::onPanningEnded(const glm::vec2& p, const glm::vec2& start) {
    o_assert_dbg(this->dispatcher);

    this->panningEnded = true;
    this->panning = false;
    this->position[0] = p;
    this->startPosition[0] = start;

    InputEvent ie;
    ie.Type = InputEvent::TouchPanningEnded;
    ie.TouchPosition[0] = this->position[0];
    ie.TouchStartPosition[0] = this->startPosition[0];
    this->dispatcher->notifyEvent(ie);
}

//------------------------------------------------------------------------------
void
touchpadDevice::onPanningCancelled() {
    o_assert_dbg(this->dispatcher);

    this->panning = false;

    InputEvent ie;
    ie.Type = InputEvent::TouchPanningCancelled;
    this->dispatcher->notifyEvent(ie);
}

//------------------------------------------------------------------------------
void
touchpadDevice::onPinchingStarted(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& s0, const glm::vec2& s1) {
    o_assert_dbg(this->dispatcher);

    this->pinchingStarted = true;
    this->pinching = true;
    this->position[0] = p0;
    this->position[1] = p1;
    this->startPosition[0] = s0;
    this->startPosition[1] = s1;

    InputEvent ie;
    ie.Type = InputEvent::TouchPinchingStarted;
    ie.TouchPosition = this->position;
    ie.TouchStartPosition = this->startPosition;
    this->dispatcher->notifyEvent(ie);
}

//------------------------------------------------------------------------------
void
touchpadDevice::onPinching(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& s0, const glm::vec2& s1) {
    o_assert_dbg(this->dispatcher);

    this->pinching = true;
    this->movement[0] = p0 - this->position[0];
    this->movement[1] = p1 - this->position[1];
    this->position[0] = p0;
    this->position[1] = p1;
    this->startPosition[0] = s0;
    this->startPosition[1] = s1;

    InputEvent ie;
    ie.Type = InputEvent::TouchPinching;
    ie.TouchPosition = this->position;
    ie.TouchMovement = this->movement;
    ie.TouchStartPosition = this->startPosition;
    this->dispatcher->notifyEvent(ie);
}

//------------------------------------------------------------------------------
void
touchpadDevice::onPinchingEnded(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& s0, const glm::vec2& s1) {
    o_assert_dbg(this->dispatcher);

    this->pinchingEnded = true;
    this->pinching = false;
    this->position[0] = p0;
    this->position[1] = p1;
    this->startPosition[0] = s0;
    this->startPosition[1] = s1;

    InputEvent ie;
    ie.Type = InputEvent::TouchPinchingEnded;
    ie.TouchPosition = this->position;
    ie.TouchStartPosition = this->startPosition;
    this->dispatcher->notifyEvent(ie);
}

//------------------------------------------------------------------------------
void
touchpadDevice::onPinchingCancelled() {
    o_assert_dbg(this->dispatcher);

    this->pinching = false;

    InputEvent ie;
    ie.Type = InputEvent::TouchPinchingCancelled;
    this->dispatcher->notifyEvent(ie);
}

//------------------------------------------------------------------------------
void
gamepadDevice::reset() {
    this->down = 0;
    this->up = 0;
    this->axes.Fill(0.0f);
}

//------------------------------------------------------------------------------
bool
gamepadDevice::buttonPressed(int rawBtnIndex) const {
    return 0 != (this->pressed & (1<<rawBtnIndex));
}

//------------------------------------------------------------------------------
bool
gamepadDevice::buttonDown(int rawBtnIndex) const {
    return 0 != (this->down & (1<<rawBtnIndex));
}

//------------------------------------------------------------------------------
bool
gamepadDevice::buttonUp(int rawBtnIndex) const {
    return 0 != (this->up & (1<<rawBtnIndex));
}

//------------------------------------------------------------------------------
float
gamepadDevice::axisValue(int rawAxisIndex) const {
    return this->axes[rawAxisIndex];
}

} // namespace _priv
} // namespace Oryol
