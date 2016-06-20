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

/// gamepad buttons, triggers, sticks
struct GamepadGizmo {
    enum Code {
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
        
        NumGamepadGizmos,
        InvalidGamepadGizmo,
    };
};

} // namespace Oryol
