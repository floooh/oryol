#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup Input Input
    @brief getting input

    @class Oryol::Input
    @ingroup Input
    @brief Input module facade
 
    Provides access to connected input devices,
    like keyboard, mouse and game pads. On mobile platforms it also allows
    to query touch gestures.
*/
#include "Input/Core/inputMgr.h"

namespace Oryol {
    
class Input {
public:
    /// setup the Input module
    static void Setup(const InputSetup& inputSetup = InputSetup());
    /// discard the Input module
    static void Discard();
    /// check if Input module is valid
    static bool IsValid();
    
    /// attach an input event handler
    static void AttachInputHandler(const Ptr<Port>& handler);
    /// detach an input event handler
    static void DetachInputHandler(const Ptr<Port>& handler);
    
    /// get keyboard state
    static const class Keyboard& Keyboard();
    /// get mouse state
    static const class Mouse& Mouse();
    /// get gamepad state
    static const class Gamepad& Gamepad(int32 index);
    /// get touchpad state (for touch/multitouch gestures)
    static const class Touchpad& Touchpad();
    /// get sensory state
    static const class Sensors& Sensors();
    
    /// set mouse cursor mode
    static void SetCursorMode(CursorMode::Code mode);
    /// get current mouse cursor mode
    static CursorMode::Code GetCursorMode();
    /// begin text capturing
    static void BeginCaptureText();
    /// end text capturing
    static void EndCaptureText();
    
private:
    struct _state {
        _priv::inputMgr inputManager;
    };
    static _state* state;
};

} // namespace Oryol