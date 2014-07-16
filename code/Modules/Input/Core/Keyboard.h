#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Input::Keyboard
    @brief access to keyboard state
*/
#include "Input/Core/Key.h"

namespace Oryol {
namespace Input {

class Keyboard {
public:
    /// constructor
    Keyboard();

    /// test if key is currently pressed
    bool Pressed(Key::Code key) const;
    /// test if key was pressed down last frame
    bool Down(Key::Code key) const;
    /// test if key was released last frame
    bool Up(Key::Code key) const;
    
    /// number of key-downs last frame
    uint8 NumDown(Key::Code key) const;
    /// number of key-ups last frame
    uint8 NumUp(Key::Code key) const;
    
    /// get the last frame's text input (will return valid ptr to 0-terminated string)
    const wchar_t* Text(uint8& outNumChars) const;
    
private:
    
};
} // namespace Input
} // namespace Oryol