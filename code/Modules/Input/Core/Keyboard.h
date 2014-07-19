#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Input::Keyboard
    @brief access to keyboard state
*/
#include "Input/Core/Key.h"
#include "Core/Assert.h"
#include <bitset>

namespace Oryol {
namespace Input {

class Keyboard {
public:
    /// constructor
    Keyboard();
    
    /// return true if keyboard is valid/attached
    bool Attached() const;
    /// test if key is currently pressed
    bool Pressed(Key::Code key) const;
    /// test if key was pressed down last frame
    bool Down(Key::Code key) const;
    /// test if key was released last frame
    bool Up(Key::Code key) const;
    
    /// get the last frame's text input (will return valid ptr to 0-terminated string)
    const wchar_t* Text(uint8& outNumChars) const;
    
    /// set keyboard attached state
    void setAttached(bool b);
    /// call when key down event happens
    void onKeyDown(Key::Code key);
    /// call when key up event happens
    void onKeyUp(Key::Code key);
    /// call when character is appended
    void onChar(wchar_t c);
    /// reset the keyboard state
    void reset();
    
private:
    std::bitset<Key::NumKeys> down;
    std::bitset<Key::NumKeys> up;
    std::bitset<Key::NumKeys> pressed;
    static const int32 MaxNumChars = 64;
    int32 charIndex;
    wchar_t chars[MaxNumChars + 1];
    bool attached;
};

//------------------------------------------------------------------------------
inline bool
Keyboard::Attached() const {
    return this->attached;
}

//------------------------------------------------------------------------------
inline bool
Keyboard::Pressed(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->pressed[key];
}

//------------------------------------------------------------------------------
inline bool
Keyboard::Down(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->down[key];
}

//------------------------------------------------------------------------------
inline bool
Keyboard::Up(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->up[key];
}

//------------------------------------------------------------------------------
inline const wchar_t*
Keyboard::Text(uint8& outNumChars) const {
    outNumChars = this->charIndex;
    return this->chars;
}
    
} // namespace Input
} // namespace Oryol
