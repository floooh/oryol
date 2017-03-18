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
#include "Core/String/StringAtom.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {
namespace _priv {
    
class gamepadDevice {
public:
    /// gamepad is valid/attached?
    bool attached = false;
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
    /// reset the gamepad state
    void reset();

    uint32_t down = 0;
    uint32_t up = 0;
    uint32_t pressed = 0;
    StaticArray<glm::vec2, GamepadGizmo::NumGamepadGizmoValues> values;
};

//------------------------------------------------------------------------------
inline bool
gamepadDevice::buttonPressed(GamepadGizmo::Code btn) const {
    return 0 != (this->pressed & btn);
}

//------------------------------------------------------------------------------
inline bool
gamepadDevice::buttonDown(GamepadGizmo::Code btn) const {
    return 0 != (this->down & btn);
}

//------------------------------------------------------------------------------
inline bool
gamepadDevice::buttonUp(GamepadGizmo::Code btn) const {
    return 0 != (this->up & btn);
}

//------------------------------------------------------------------------------
inline float
gamepadDevice::triggerValue(GamepadGizmo::Value trigger) const {
    return this->values[trigger].x;
}

//------------------------------------------------------------------------------
inline const glm::vec2&
gamepadDevice::stickPos(GamepadGizmo::Value stick) const {
    return this->values[stick];
}

} // namespace _priv
} // namespace Oryol
