#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::InputEnums
    @brief input module enumeration types
*/
#include "Core/Types.h"

namespace Oryol {

/// mouse buttons
struct MouseButton {
    enum Code {
        LMB = 0,
        MMB,
        RMB,
        
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
