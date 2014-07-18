//------------------------------------------------------------------------------
//  Keyboard.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Keyboard.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace Input {

using namespace Core;
    
//------------------------------------------------------------------------------
Keyboard::Keyboard() :
charIndex(0),
attached(false) {
    Memory::Clear(&this->keyState, sizeof(this->keyState));
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
    this->keyState[key] |= (keyDown | keyPressed);
}

//------------------------------------------------------------------------------
void
Keyboard::onKeyUp(Key::Code key) {
    o_assert_range_dbg(key, Key::NumKeys);
    this->keyState[key] &= ~(keyDown | keyPressed);
    this->keyState[key] |= keyUp;
}

//------------------------------------------------------------------------------
void
Keyboard::onChar(wchar_t c) {
    if (this->charIndex < MaxNumChars) {
        this->chars[charIndex++] = c;
        this->chars[charIndex] = 0;
    }
}

//------------------------------------------------------------------------------
void
Keyboard::reset() {
    this->charIndex = 0;
    this->chars[0] = 0;
    for (int32 i = 0; i < Key::NumKeys; i++) {
        this->keyState[i] &= ~(keyDown | keyUp);
    }
}

} // namespace Input
} // namespace Oryol