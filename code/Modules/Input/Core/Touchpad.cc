//------------------------------------------------------------------------------
//  Touchpad.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "Input/Core/Touchpad.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
Touchpad::Touchpad() :
Attached(false),
Tapped(false),
DoubleTapped(false),
PanningStarted(false),
Panning(false),
PanningEnded(false),
PinchingStarted(false),
Pinching(false),
PinchingEnded(false) {
    // empty
}

//------------------------------------------------------------------------------
void
Touchpad::subscribe(const StringAtom& id, EventHandler handler) {
    o_assert_dbg(!this->eventHandlers.Contains(id));
    this->eventHandlers.Add(id, handler);
}

//------------------------------------------------------------------------------
void
Touchpad::unsubscribe(const StringAtom& id) {
    if (this->eventHandlers.Contains(id)) {
        this->eventHandlers.Erase(id);
    }
}

//------------------------------------------------------------------------------
void
Touchpad::notifyEventHandlers(const Event& event) {
    for (const auto& entry : this->eventHandlers) {
        entry.Value()(event);
    }
}

//------------------------------------------------------------------------------
void
Touchpad::reset() {
    this->Tapped = false;
    this->DoubleTapped = false;
    this->PanningStarted = false;
    this->PanningEnded = false;
    this->PinchingStarted = false;
    this->PinchingEnded = false;
    for (int i = 0; i < MaxNumTouches; i++) {
        this->Movement[i] = glm::vec2(0.0f, 0.0f);
    }
}

//------------------------------------------------------------------------------
void
Touchpad::onTapped(const glm::vec2& p) {
    this->Tapped = true;
    this->Position[0] = p;
    this->StartPosition[0] = p;
    if (!this->eventHandlers.Empty()) {
        Event event;
        event.Type = Event::Tapped;
        event.Position[0] = this->Position[0];
        event.StartPosition[0] = this->StartPosition[0];
        this->notifyEventHandlers(event);
    }
}

//------------------------------------------------------------------------------
void
Touchpad::onDoubleTapped(const glm::vec2& p) {
    this->DoubleTapped = true;
    this->Position[0] = p;
    this->StartPosition[0] = p;
    if (!this->eventHandlers.Empty()) {
        Event event;
        event.Type = Event::DoubleTapped;
        event.Position[0] = this->Position[0];
        event.StartPosition[0] = this->StartPosition[0];
        this->notifyEventHandlers(event);
    }
}

//------------------------------------------------------------------------------
void
Touchpad::onPanningStarted(const glm::vec2& p, const glm::vec2& start) {
    this->PanningStarted = true;
    this->Panning = true;
    this->Position[0] = p;
    this->StartPosition[0] = start;
    if (!this->eventHandlers.Empty()) {
        Event event;
        event.Type = Event::PanningStarted;
        event.Position[0] = this->Position[0];
        event.StartPosition[0] = this->StartPosition[0];
        this->notifyEventHandlers(event);
    }
}

//------------------------------------------------------------------------------
void
Touchpad::onPanning(const glm::vec2& p, const glm::vec2& start) {
    this->Panning = true;
    this->Movement[0] = p - this->Position[0];
    this->Position[0] = p;
    this->StartPosition[0] = start;
    if (!this->eventHandlers.Empty()) {
        Event event;
        event.Type = Event::Panning;
        event.Position[0] = this->Position[0];
        event.Movement[0] = this->Movement[0];
        event.StartPosition[0] = this->StartPosition[0];
        this->notifyEventHandlers(event);
    }
}

//------------------------------------------------------------------------------
void
Touchpad::onPanningEnded(const glm::vec2& p, const glm::vec2& start) {
    this->PanningEnded = true;
    this->Panning = false;
    this->Position[0] = p;
    this->StartPosition[0] = start;
    if (!this->eventHandlers.Empty()) {
        Event event;
        event.Type = Event::PanningEnded;
        event.Position[0] = this->Position[0];
        event.StartPosition[0] = this->StartPosition[0];
        this->notifyEventHandlers(event);
    }
}

//------------------------------------------------------------------------------
void
Touchpad::onPanningCancelled() {
    this->Panning = false;
    if (!this->eventHandlers.Empty()) {
        Event event;
        event.Type = Event::PanningCancelled;
        this->notifyEventHandlers(event);
    }
}

//------------------------------------------------------------------------------
void
Touchpad::onPinchingStarted(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& s0, const glm::vec2& s1) {
    this->PinchingStarted = true;
    this->Pinching = true;
    this->Position[0] = p0;
    this->Position[1] = p1;
    this->StartPosition[0] = s0;
    this->StartPosition[1] = s1;
    if (!this->eventHandlers.Empty()) {
        Event event;
        event.Type = Event::PinchingStarted;
        event.Position = this->Position;
        event.StartPosition = this->StartPosition;
        this->notifyEventHandlers(event);
    }
}

//------------------------------------------------------------------------------
void
Touchpad::onPinching(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& s0, const glm::vec2& s1) {
    this->Pinching = true;
    this->Movement[0] = p0 - this->Position[0];
    this->Movement[1] = p1 - this->Position[1];
    this->Position[0] = p0;
    this->Position[1] = p1;
    this->StartPosition[0] = s0;
    this->StartPosition[1] = s1;
    if (!this->eventHandlers.Empty()) {
        Event event;
        event.Type = Event::Pinching;
        event.Position = this->Position;
        event.Movement = this->Movement;
        event.StartPosition = this->StartPosition;
        this->notifyEventHandlers(event);
    }
}

//------------------------------------------------------------------------------
void
Touchpad::onPinchingEnded(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& s0, const glm::vec2& s1) {
    this->PinchingEnded = true;
    this->Pinching = false;
    this->Position[0] = p0;
    this->Position[1] = p1;
    this->StartPosition[0] = s0;
    this->StartPosition[1] = s1;
    if (!this->eventHandlers.Empty()) {
        Event event;
        event.Type = Event::PinchingEnded;
        event.Position = this->Position;
        event.StartPosition = this->StartPosition;
        this->notifyEventHandlers(event);
    }
}

//------------------------------------------------------------------------------
void
Touchpad::onPinchingCancelled() {
    this->Pinching = false;
    if (!this->eventHandlers.Empty()) {
        Event event;
        event.Type = Event::PinchingCancelled;
        this->notifyEventHandlers(event);
    }
}

} // namespace Oryol
