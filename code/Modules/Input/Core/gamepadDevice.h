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
    bool buttonPressed(GamepadButton::Code btn) const;
    /// test if button was pressed-down this frame
    bool buttonDown(GamepadButton::Code btn) const;
    /// test if button was released this frame
    bool buttonUp(GamepadButton::Code btn) const;
    /// get axes value (-1.0 to 1.0 for sticks, 0.0 to 1.0 for triggers)
    float axisValue(GamepadAxis::Code axis) const;
    /// reset the gamepad state
    void reset();

    StringAtom id;
    uint32_t down = 0;
    uint32_t up = 0;
    uint32_t pressed = 0;
    StaticArray<float, GamepadAxis::NumAxes> axes;
    struct Mapping {
        Mapping() {
            for (uint32_t i = 0; i < GamepadButton::NumButtons; i++) {
                this->buttons[i] = (1<<i);
            }
            for (uint32_t i = 0; i < GamepadAxis::NumAxes; i++) {
                this->axes[i].axisIndex = i;
            }
        }
        struct axisMapping {
            uint32_t axisIndex = 0;
            float scale = 1.0f;
            float bias  = 0.0f;
        };
        StaticArray<uint32_t, GamepadButton::NumButtons> buttons;
        StaticArray<axisMapping, GamepadAxis::NumAxes> axes;
    };
    Mapping mapping;
};

//------------------------------------------------------------------------------
inline bool
gamepadDevice::buttonPressed(GamepadButton::Code btn) const {
    return 0 != (this->pressed & (1<<btn));
}

//------------------------------------------------------------------------------
inline bool
gamepadDevice::buttonDown(GamepadButton::Code btn) const {
    return 0 != (this->down & (1<<btn));
}

//------------------------------------------------------------------------------
inline bool
gamepadDevice::buttonUp(GamepadButton::Code btn) const {
    return 0 != (this->up & (1<<btn));
}

//------------------------------------------------------------------------------
inline float
gamepadDevice::axisValue(GamepadAxis::Code axis) const {
    return this->axes[axis];
}

} // namespace _priv
} // namespace Oryol
