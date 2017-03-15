#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::gamepadDevice
    @ingroup _priv
    @brief access to gamepad state
*/
#include "Core/Assertion.h"
#include "glm/vec2.hpp"
#include "Input/Core/InputEnums.h"

namespace Oryol {
namespace _priv {
    
class gamepadDevice {
private:
    //static const unsigned char NUMVALUESMAX = 64;
public:
    /// constructor
    gamepadDevice();
    
    /// gamepad is valid/attached?
    bool attached;
    /// test if button is currently pressed
    bool buttonPressed(GamepadGizmo::Code btn) const;
    /// test if button was pressed-down this frame
    bool buttonDown(GamepadGizmo::Code btn) const;
    /// test if button was released this frame
    bool buttonUp(GamepadGizmo::Code btn) const;
    /// get trigger position (0.0 .. 1.0)
    float triggerValue(GamepadGizmo::Value trigger) const;
    /// get stick position (-1.0 .. +1.0)
    const glm::vec2& stickPos(GamepadGizmo::Value stick) const;
    
    /// called when button is pressed-down
    void onButtonDown(GamepadGizmo::Code btn);
    /// called when button is released
    void onButtonUp(GamepadGizmo::Code btn);
    /// update trigger value
    void onTriggerValue(GamepadGizmo::Value trigger, float val);
    /// update stick position
    void onStickPos(GamepadGizmo::Value stick, const glm::vec2& pos);
    /// reset the gamepad state
    void reset();
    
    uint32_t down;
    uint32_t up;
    uint32_t pressed;
    // TODO: What is the true maximum among supported platforms and devices?
    glm::vec2 values[GamepadGizmo::NumGamepadGizmoValues];
};

//------------------------------------------------------------------------------
inline bool
gamepadDevice::buttonPressed(GamepadGizmo::Code btn) const {
    o_assert(((0 == btn % 2) || (1 == btn)) && (btn <= GamepadGizmo::MaxGamepadGizmoCode));
    return 0 != (this->pressed & btn);
}

//------------------------------------------------------------------------------
inline bool
gamepadDevice::buttonDown(GamepadGizmo::Code btn) const {
    o_assert(((0 == btn % 2) || (1 == btn)) && (btn <= GamepadGizmo::MaxGamepadGizmoCode));
    return 0 != (this->down & btn);
}

//------------------------------------------------------------------------------
inline bool
gamepadDevice::buttonUp(GamepadGizmo::Code btn) const {
    o_assert(((0 == btn % 2) || (1 == btn)) && (btn <= GamepadGizmo::MaxGamepadGizmoCode));
    return 0 != (this->up & btn);
}

//------------------------------------------------------------------------------
inline float
gamepadDevice::triggerValue(GamepadGizmo::Value trigger) const {
    o_assert_range_dbg(trigger, GamepadGizmo::NumGamepadGizmoValues);
    return this->values[trigger].x;
}

//------------------------------------------------------------------------------
inline const glm::vec2&
gamepadDevice::stickPos(GamepadGizmo::Value stick) const {
    o_assert_range_dbg(stick, GamepadGizmo::NumGamepadGizmoValues);
    return this->values[stick];
}

} // namespace _priv
} // namespace Oryol
