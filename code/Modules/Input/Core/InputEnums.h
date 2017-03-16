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
        InvalidGamepadGizmo = 0,
        A                   = (1 << 0),
        B                   = (1 << 1),
        X                   = (1 << 2),
        Y                   = (1 << 3),

        Start               = (1 << 4),
        Back                = (1 << 5),

        RightBumper         = (1 << 6),
        LeftBumper          = (1 << 7),

        DPadLeft            = (1 << 8),
        DPadRight           = (1 << 9),
        DPadUp              = (1 << 10),
        DPadDown            = (1 << 11),

        LeftTrigger         = (1 << 12),
        RightTrigger        = (1 << 13),
        LeftStick           = (1 << 14),
        RightStick          = (1 << 15),
        MaxGamepadGizmoCode    = RightStick
    };
    enum Value {
        LeftTriggerValue        = 0,
        RightTriggerValue       = 1,
        LeftStickValue          = 2,
        RightStickValue         = 3,
        NumGamepadGizmoValues   = 4
    };
};

} // namespace Oryol
