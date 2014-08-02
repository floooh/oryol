#pragma once
//------------------------------------------------------------------------------
/**
    @class Input::inputMgrBase
    @brief base class for platform-specific input managers
*/
#include "Input/Core/Keyboard.h"
#include "Input/Core/Mouse.h"
#include "Input/Core/Gamepad.h"
#include "Input/Core/Touchpad.h"

namespace Oryol {
namespace Input {
    
class inputMgrBase {
public:
    /// constructor
    inputMgrBase();
    /// destructor
    ~inputMgrBase();

    /// get keyboard state
    const class Keyboard& Keyboard() const;
    /// get mouse state
    const class Mouse& Mouse() const;
    /// get gamepad state
    const class Gamepad& Gamepad(int32 index) const;
    /// get touchpad state (for touch/multitouch gestures)
    const class Touchpad& Touchpad() const;

protected:
    static const int32 MaxNumGamepads = 4;
    class Keyboard keyboard;
    class Mouse mouse;
    class Gamepad gamepads[MaxNumGamepads];
    class Touchpad touchpad;
};

//------------------------------------------------------------------------------
const class Keyboard&
inputMgrBase::Keyboard() const {
    return this->keyboard;
}

//------------------------------------------------------------------------------
const class Mouse&
inputMgrBase::Mouse() const {
    return this->mouse;
}

//------------------------------------------------------------------------------
const class Gamepad&
inputMgrBase::Gamepad(int32 index) const {
    o_assert_range(index, MaxNumGamepads);
    return this->gamepads[index];
}

//------------------------------------------------------------------------------
const class Touchpad&
inputMgrBase::Touchpad() const {
    return this->touchpad;
}

} // namespace Input
} // namespace Oryol



