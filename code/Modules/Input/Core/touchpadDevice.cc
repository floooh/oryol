//------------------------------------------------------------------------------
//  touchpadDevice.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "touchpadDevice.h"
#include "inputDispatcher.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
touchpadDevice::touchpadDevice() :
attached(false),
tapped(false),
doubleTapped(false),
panningStarted(false),
panning(false),
panningEnded(false),
pinchingStarted(false),
pinching(false),
pinchingEnded(false),
dispatcher(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
void
touchpadDevice::reset() {
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

} // namespace _priv
} // namespace Oryol
