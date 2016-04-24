//------------------------------------------------------------------------------
//  Mouse.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Mouse.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
Mouse::Mouse() :
Attached(false),
Position(0.0f, 0.0f),
Movement(0.0f, 0.0f),
Scroll(0.0f, 0.0f),
uniqueIdCounter(0) {
    for (int i = 0; i < Button::NumButtons; i++) {
        this->buttonState[i] = 0;
    }
}

//------------------------------------------------------------------------------
Mouse::EventHandlerId
Mouse::subscribe(EventHandler handler) {
    EventHandlerId id = this->uniqueIdCounter++;
    this->eventHandlers.Add(id, handler);
    return id;
}

//------------------------------------------------------------------------------
void
Mouse::unsubscribe(EventHandlerId id) {
    if (this->eventHandlers.Contains(id)) {
        this->eventHandlers.Erase(id);
    }
}

//------------------------------------------------------------------------------
void
Mouse::setPointerLockHandler(PointerLockHandler handler) {
    o_assert_dbg(!this->pointerLockHandler);
    this->pointerLockHandler = handler;
}

//------------------------------------------------------------------------------
void
Mouse::clearPointerLockHandler() {
    this->pointerLockHandler = PointerLockHandler();
}

//------------------------------------------------------------------------------
void
Mouse::notifyEventHandlers(const Event& event) {
    for (const auto& entry : this->eventHandlers) {
        entry.Value()(event);
    }
}

//------------------------------------------------------------------------------
Mouse::PointerLockMode
Mouse::notifyPointerLockHandler(const Event& event) {
    if (this->pointerLockHandler) {
        return this->pointerLockHandler(event);
    }
    else {
        return PointerLockModeDontCare;
    }
}

//------------------------------------------------------------------------------
Mouse::PointerLockMode
Mouse::onButtonDown(Button btn) {
    o_assert_range_dbg(btn, NumButtons);
    this->buttonState[btn] |= (btnDown | btnPressed);
    Event event(Event::ButtonDown, btn);
    if (!this->eventHandlers.Empty()) {
        this->notifyEventHandlers(event);
    }
    return this->notifyPointerLockHandler(event);
}

//------------------------------------------------------------------------------
Mouse::PointerLockMode
Mouse::onButtonUp(Button btn) {
    o_assert_range_dbg(btn, NumButtons);
    this->buttonState[btn] &= ~btnPressed;
    this->buttonState[btn] |= btnUp;
    Event event(Event::ButtonUp, btn);
    if (!this->eventHandlers.Empty()) {
        this->notifyEventHandlers(event);
    }
    return this->notifyPointerLockHandler(event);
}

//------------------------------------------------------------------------------
void
Mouse::onPosMov(const glm::vec2& pos) {
    this->Movement = pos - this->Position;
    this->Position = pos;
    if (!this->eventHandlers.Empty()) {
        this->notifyEventHandlers(Event(Event::Move, this->Movement, this->Position));
    }
}

//------------------------------------------------------------------------------
void
Mouse::onPosMov(const glm::vec2& pos, const glm::vec2& mov) {
    this->Movement = mov;
    this->Position = pos;
    if (!this->eventHandlers.Empty()) {
        this->notifyEventHandlers(Event(Event::Move, this->Movement, this->Position));
    }
}

//------------------------------------------------------------------------------
void
Mouse::onMov(const glm::vec2& mov) {
    this->Movement = mov;
    if (!this->eventHandlers.Empty()) {
        this->notifyEventHandlers(Event(Event::Move, this->Movement, this->Position));
    }
}

//------------------------------------------------------------------------------
void
Mouse::onScroll(const glm::vec2& scroll) {
    this->Scroll = scroll;
    if (!this->eventHandlers.Empty()) {
        this->notifyEventHandlers(Event(Event::Scroll, this->Scroll));
    }
}

//------------------------------------------------------------------------------
void
Mouse::reset() {
    for (int i = 0; i < NumButtons; i++) {
        this->buttonState[i] &= ~(btnDown | btnUp);
    }
    this->Movement = glm::vec2(0.0f, 0.0f);
    this->Scroll = glm::vec2(0.0f, 0.0f);
}

} // namespace Oryol
