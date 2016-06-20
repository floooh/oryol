#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::keyboardDevice
    @ingroup _priv
    @brief access to keyboard state
*/
#include "Input/Core/Key.h"
#include "Core/Assertion.h"
#include <bitset>

namespace Oryol {
namespace _priv {

class inputDispatcher;

class keyboardDevice {
public:
    /// constructor
    keyboardDevice();

    /// keyboard attached?
    bool attached;
    /// test if key is currently pressed
    bool keyPressed(Key::Code key) const;
    /// test if key was pressed down last frame
    bool keyDown(Key::Code key) const;
    /// test if key was released last frame
    bool keyUp(Key::Code key) const;
    /// test if key-repeat happened
    bool keyRepeat(Key::Code key) const;
    /// test if any key is pressed
    bool anyKeyPressed() const;
    /// test if any key was pressed down last frame
    bool anyKeyDown() const;
    /// test if any key was released last frame
    bool anyKeyUp() const;
    /// test if any key was repeated last frame
    bool anyKeyRepeat() const;
    
    /// get current captured text
    const wchar_t* capturedText() const;

    /// call when key down event happens
    void onKeyDown(Key::Code key);
    /// call when key up event happens
    void onKeyUp(Key::Code key);
    /// call when key repeat event happens
    void onKeyRepeat(Key::Code key);
    /// call when character is appended
    void onChar(wchar_t c);
    /// reset the keyboard state
    void reset();
    /// clear captured text (called in reset())
    void clearCapturedText();

    inputDispatcher* dispatcher;
    std::bitset<Key::NumKeys> down;
    std::bitset<Key::NumKeys> up;
    std::bitset<Key::NumKeys> pressed;
    std::bitset<Key::NumKeys> repeat;
    static const int MaxNumChars = 128;
    int charIndex;
    wchar_t chars[MaxNumChars + 1];
};

//------------------------------------------------------------------------------
inline bool
keyboardDevice::keyPressed(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->pressed[key];
}

//------------------------------------------------------------------------------
inline bool
keyboardDevice::keyDown(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->down[key];
}

//------------------------------------------------------------------------------
inline bool
keyboardDevice::keyUp(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->up[key];
}

//------------------------------------------------------------------------------
inline bool
keyboardDevice::keyRepeat(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->repeat[key];
}

//------------------------------------------------------------------------------
inline bool
keyboardDevice::anyKeyPressed() const {
    return this->pressed.any();
}

//------------------------------------------------------------------------------
inline bool
keyboardDevice::anyKeyDown() const {
    return this->down.any();
}

//------------------------------------------------------------------------------
inline bool
keyboardDevice::anyKeyUp() const {
    return this->up.any();
}

//------------------------------------------------------------------------------
inline bool
keyboardDevice::anyKeyRepeat() const {
    return this->repeat.any();
}

//------------------------------------------------------------------------------
inline const wchar_t*
keyboardDevice::capturedText() const {
    return this->chars;
}

} // namespace _priv
} // namespace Oryol

