//------------------------------------------------------------------------------
//  Keyboard.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Keyboard.h"
#include "Core/Memory/Memory.h"

namespace Oryol {

//------------------------------------------------------------------------------
Keyboard::Keyboard() :
charIndex(0),
attached(false),
textCapturing(false) {
    Memory::Clear(&this->chars, sizeof(this->chars));
}

//------------------------------------------------------------------------------
void
Keyboard::setAttached(bool b) {
    this->attached = b;
}

//------------------------------------------------------------------------------
void
Keyboard::onKeyDown(Key::Code key) {
    o_assert_range_dbg(key, Key::NumKeys);
    this->down[key] = true;
    this->pressed[key] = true;
}

//------------------------------------------------------------------------------
void
Keyboard::onKeyUp(Key::Code key) {
    o_assert_range_dbg(key, Key::NumKeys);
    this->up[key] = true;
    this->pressed[key] = false;
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
}

//------------------------------------------------------------------------------
void
Keyboard::beginCaptureText() {
    o_assert(!this->textCapturing);
    this->textCapturing = true;
    this->chars[0] = 0;
    this->charIndex = 0;
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
}

} // namespace Oryol
