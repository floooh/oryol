//------------------------------------------------------------------------------
//  keyboardDevice.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "keyboardDevice.h"
#include "Core/Memory/Memory.h"
#include "Input/Core/inputDispatcher.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
keyboardDevice::keyboardDevice() :
attached(false),
dispatcher(nullptr),
charIndex(0) {
    Memory::Clear(&this->chars, sizeof(this->chars));
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

} // namespace _priv
} // namespace Oryol
