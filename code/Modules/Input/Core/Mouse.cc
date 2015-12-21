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
Scroll(0.0f, 0.0f) {
    for (int32 i = 0; i < Button::NumButtons; i++) {
        this->buttonState[i] = 0;
    }
}

//------------------------------------------------------------------------------
void
Mouse::subscribe(const StringAtom& id, EventHandler handler) {
    o_assert_dbg(!this->eventHandlers.Contains(id));
    this->eventHandlers.Add(id, handler);
}

//------------------------------------------------------------------------------
void
Mouse::unsubscribe(const StringAtom& id) {
    if (this->eventHandlers.Contains(id)) {
        this->eventHandlers.Erase(id);
    }
}

//------------------------------------------------------------------------------
void
Mouse::notifyEventHandlers(const Event& event) {
    for (const auto& entry : this->eventHandlers) {
        entry.Value()(event);
    }
}

//------------------------------------------------------------------------------
void
Mouse::onButtonDown(Button btn) {
    o_assert_range_dbg(btn, NumButtons);
    this->buttonState[btn] |= (btnDown | btnPressed);
    if (!this->eventHandlers.Empty()) {
        this->notifyEventHandlers(Event(Event::ButtonDown, btn));
    }
}

//------------------------------------------------------------------------------
void
Mouse::onButtonUp(Button btn) {
    o_assert_range_dbg(btn, NumButtons);
    this->buttonState[btn] &= ~btnPressed;
    this->buttonState[btn] |= btnUp;
    if (!this->eventHandlers.Empty()) {
        this->notifyEventHandlers(Event(Event::ButtonUp, btn));
    }
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
    for (int32 i = 0; i < NumButtons; i++) {
        this->buttonState[i] &= ~(btnDown | btnUp);
    }
    this->Movement = glm::vec2(0.0f, 0.0f);
    this->Scroll = glm::vec2(0.0f, 0.0f);
}

} // namespace Oryol
