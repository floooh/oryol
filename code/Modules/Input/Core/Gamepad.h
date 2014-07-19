#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Input::Gamepad
    @brief access to gamepad state
*/
#include "Core/Assert.h"
#include "glm/vec2.hpp"
#include <bitset>
#include <array>

namespace Oryol {
namespace Input {
    
class Gamepad {
public:
    /// constructor
    Gamepad();
    
    /// gamepad buttons, triggers, sticks
    enum Element {
        A = 0,
        B,
        X,
        Y,
        
        Start,
        Back,
        
        RightBumper,
        LeftBumper,
        
        DPadLeft,
        DPadRight,
        DPadUp,
        DPadDown,
        
        LeftTrigger,
        RightTrigger,
        LeftStick,
        RightStick,
        
        NumElements,
        InvalidElement = 0,
    };
    
    /// return true if gamepad is valid/attached
    bool Attached() const;
    /// test if button is currently pressed
    bool ButtonPressed(Element btn) const;
    /// test if button was pressed-down this frame
    bool ButtonDown(Element btn) const;
    /// test if button was released this frame
    bool ButtonUp(Element btn) const;
    /// get trigger position (0.0 .. 1.0)
    float32 TriggerValue(Element trigger) const;
    /// get stick position (-1.0 .. +1.0)
    const glm::vec2& StickPos(Element stick) const;
    
    /// FIXME??? called when button is pressed-down
    void onButtonDown(Element btn);
    /// FIXME??? called when button is released
    void onButtonUp(Element btn);
    /// update trigger value
    void onTriggerValue(Element trigger, float32 val);
    /// update stick position
    void onStickPos(Element stick, const glm::vec2& pos);
    /// reset the gamepad state
    void reset();
    
private:
    uint32 down;
    uint32 up;
    uint32 pressed;
    std::array<glm::vec2, NumElements> values;
    bool attached;
};

//------------------------------------------------------------------------------
inline bool
Gamepad::Attached() const {
    return this->attached;
}

//------------------------------------------------------------------------------
inline bool
Gamepad::ButtonPressed(Element btn) const {
    o_assert_range_dbg(btn, NumElements);
    return 0 != (this->pressed & (1<<btn));
}

//------------------------------------------------------------------------------
inline bool
Gamepad::ButtonDown(Element btn) const {
    o_assert_range_dbg(btn, NumElements);
    return 0 != (this->down & (1<<btn));
}

//------------------------------------------------------------------------------
inline bool
Gamepad::ButtonUp(Element btn) const {
    o_assert_range_dbg(btn, NumElements);
    return 0 != (this->up & (1<<btn));
}

//------------------------------------------------------------------------------
inline float
Gamepad::TriggerValue(Element trigger) const {
    o_assert_range_dbg(trigger, NumElements);
    return this->values[trigger].x;
}

//------------------------------------------------------------------------------
inline const glm::vec2&
Gamepad::StickPos(Element stick) const {
    o_assert_range_dbg(stick, NumElements);
    return this->values[stick];
}
    
} // namespace Input
} // namespace Oryol