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
    bool KeyPressed(Key::Code key) const;
    /// test if key was pressed down last frame
    bool KeyDown(Key::Code key) const;
    /// test if key was released last frame
    bool KeyUp(Key::Code key) const;
    
    /// return true if text capturing is active
    bool IsCapturingText() const;
    /// get current captured text
    const wchar_t* CapturedText() const;
    
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
    /// begin text capturing
    void beginCaptureText();
    /// end text capturing
    void endCaptureText();
    
private:
    std::bitset<Key::NumKeys> down;
    std::bitset<Key::NumKeys> up;
    std::bitset<Key::NumKeys> pressed;
    static const int32 MaxNumChars = 128;
    int32 charIndex;
    wchar_t chars[MaxNumChars + 1];
    bool attached;
    bool textCapturing;
};

//------------------------------------------------------------------------------
inline bool
Keyboard::Attached() const {
    return this->attached;
}

//------------------------------------------------------------------------------
inline bool
Keyboard::KeyPressed(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->pressed[key];
}

//------------------------------------------------------------------------------
inline bool
Keyboard::KeyDown(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->down[key];
}

//------------------------------------------------------------------------------
inline bool
Keyboard::KeyUp(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->up[key];
}

//------------------------------------------------------------------------------
inline bool
Keyboard::IsCapturingText() const {
    return this->textCapturing;
}

//------------------------------------------------------------------------------
inline const wchar_t*
Keyboard::CapturedText() const {
    return this->chars;
}
    
} // namespace Input
} // namespace Oryol
