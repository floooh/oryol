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
#include "Input/Core/GamepadMapping.h"

namespace Oryol {
namespace _priv {
    
class gamepadDevice {
public:
    /// gamepad is valid/attached?
    bool attached = false;
    /// test if button is currently pressed
    bool buttonPressed(int rawBtnIndex) const;
    /// test if button was pressed-down this frame
    bool buttonDown(int rawBtnIndex) const;
    /// test if button was released this frame
    bool buttonUp(int rawBtnIndex) const;
    /// get axes value (-1.0 to 1.0 for sticks, 0.0 to 1.0 for triggers)
    float axisValue(int rawAxisIndex) const;
    /// reset the gamepad state
    void reset();

    static const int MaxNumRawButtons = 32;
    static const int MaxNumRawAxes = 32;
    StringAtom id;
    uint32_t down = 0;
    uint32_t up = 0;
    uint32_t pressed = 0;
    StaticArray<float, MaxNumRawAxes> axes;
    GamepadMapping mapping;
};

//------------------------------------------------------------------------------
inline void
gamepadDevice::reset() {
    this->down = 0;
    this->up = 0;
    this->axes.Fill(0.0f);
}

//------------------------------------------------------------------------------
inline bool
gamepadDevice::buttonPressed(int rawBtnIndex) const {
    return 0 != (this->pressed & (1<<rawBtnIndex));
}

//------------------------------------------------------------------------------
inline bool
gamepadDevice::buttonDown(int rawBtnIndex) const {
    return 0 != (this->down & (1<<rawBtnIndex));
}

//------------------------------------------------------------------------------
inline bool
gamepadDevice::buttonUp(int rawBtnIndex) const {
    return 0 != (this->up & (1<<rawBtnIndex));
}

//------------------------------------------------------------------------------
inline float
gamepadDevice::axisValue(int rawAxisIndex) const {
    return this->axes[rawAxisIndex];
}

} // namespace _priv
} // namespace Oryol
