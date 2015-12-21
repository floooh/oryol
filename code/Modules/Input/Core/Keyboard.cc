//------------------------------------------------------------------------------
//  Keyboard.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Keyboard.h"
#include "Core/Memory/Memory.h"

namespace Oryol {

//------------------------------------------------------------------------------
Keyboard::Keyboard() :
Attached(false),
charIndex(0),
textCapturing(false) {
    Memory::Clear(&this->chars, sizeof(this->chars));
}

//------------------------------------------------------------------------------
void
Keyboard::subscribe(const StringAtom& id, EventHandler handler) {
    o_assert_dbg(!this->eventHandlers.Contains(id));
    this->eventHandlers.Add(id, handler);
}

//------------------------------------------------------------------------------
void
Keyboard::unsubscribe(const StringAtom& id) {
    if (this->eventHandlers.Contains(id)) {
        this->eventHandlers.Erase(id);
    }
}

//------------------------------------------------------------------------------
void
Keyboard::notifyEventHandlers(const Event& event) {
    for (const auto& entry : this->eventHandlers) {
        entry.Value()(event);
    }
}

//------------------------------------------------------------------------------
void
Keyboard::onKeyDown(Key::Code key) {
    o_assert_range_dbg(key, Key::NumKeys);
    this->down[key] = true;
    this->pressed[key] = true;
    if (!this->eventHandlers.Empty()) {
        this->notifyEventHandlers(Event(Event::KeyDown, key));
    }
}

//------------------------------------------------------------------------------
void
Keyboard::onKeyUp(Key::Code key) {
    o_assert_range_dbg(key, Key::NumKeys);
    this->up[key] = true;
    this->pressed[key] = false;
    if (!this->eventHandlers.Empty()) {
        this->notifyEventHandlers(Event(Event::KeyUp, key));
    }
}

//------------------------------------------------------------------------------
void
Keyboard::onKeyRepeat(Key::Code key) {
    o_assert_range_dbg(key, Key::NumKeys);
    this->repeat[key] = true;
    if (!this->eventHandlers.Empty()) {
        this->notifyEventHandlers(Event(Event::KeyRepeat, key));
    }
}

//------------------------------------------------------------------------------
void
Keyboard::onChar(wchar_t c) {
    if (this->textCapturing) {
        if (this->charIndex < MaxNumChars) {
            this->chars[this->charIndex++] = c;
            this->chars[this->charIndex] = 0;
        }
    }
    if (!this->eventHandlers.Empty()) {
        this->notifyEventHandlers(Event(Event::WChar, c));
    }
}

//------------------------------------------------------------------------------
void
Keyboard::clearCapturedText() {
    this->chars[0] = 0;
    this->charIndex = 0;
}

//------------------------------------------------------------------------------
void
Keyboard::beginCaptureText() {
    o_assert(!this->textCapturing);
    this->textCapturing = true;
    this->clearCapturedText();
}

//------------------------------------------------------------------------------
void
Keyboard::endCaptureText() {
    o_assert(this->textCapturing);
    this->textCapturing = false;
}

//------------------------------------------------------------------------------
void
Keyboard::reset() {
    this->down.reset();
    this->up.reset();
    this->repeat.reset();
    this->clearCapturedText();
}

} // namespace Oryol
