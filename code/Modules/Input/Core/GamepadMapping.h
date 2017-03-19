#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::GamepadMapping
    @brief map raw gamepad buttons and axes to 'semantic' buttons and axes
*/
#include "Input/Core/InputEnums.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {

class GamepadMapping {
public:
    /// lookup raw button index by 'semantic' button
    StaticArray<int, GamepadButton::NumButtons> Buttons;
    /// axis mapping also contains scale/bias values
    struct AxisMapping {
        int AxisIndex = InvalidIndex;
        float Scale = 1.0f;
        float Bias = 0.0f;
        // FIXME: deadzone, origin mapping?
    };
    /// map raw axes (item index) to virtual axes (item value)
    StaticArray<AxisMapping, GamepadAxis::NumAxes> Axes;

    /// constructor
    GamepadMapping() {
        this->Buttons.Fill(InvalidIndex);
    };
};

} // namespace Oryol
