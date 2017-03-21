#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::InputEnums
    @brief input module enumeration types
*/
#include "Core/Types.h"

namespace Oryol {

/// (mouse) buttons
struct MouseButton {
    enum Code {
        Left = 0,
        Middle,
        Right,
        
        NumMouseButtons,
        InvalidMouseButton,
    };
};

/// pointer lock modes
struct PointerLockMode {
    enum Code {
        DontCare,
        Enable,
        Disable,
    };
};

/// gamepad buttons, triggers, sticks (see standard mapping: https://w3c.github.io/gamepad/)
struct GamepadButton {
    enum Code {
        A = 0,
        B,
        X,
        Y,
        LeftBumper,
        RightBumper,
        LeftTrigger,
        RightTrigger,
        Back,
        Start,
        LeftStick,
        RightStick,
        DPadUp,
        DPadDown,
        DPadLeft,
        DPadRight,
        Center,         // Xbox-button or DS4 touchpad

        NumButtons,
        InvalidButton
    };
};

struct GamepadAxis {
    enum Code {
        LeftStickHori = 0,
        LeftStickVert,
        RightStickHori,
        RightStickVert,
        LeftTrigger,
        RightTrigger,

        NumAxes,
        InvalidAxis
    };
};

} // namespace Oryol
