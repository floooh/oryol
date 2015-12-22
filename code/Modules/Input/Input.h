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

    /// subscribe to keyboard events
    static Keyboard::EventHandlerId SubscribeKeyboard(Keyboard::EventHandler handler);
    /// unsubscribe from keyboard events
    static void UnsubscribeKeyboard(Keyboard::EventHandlerId id);
    /// subscribe to mouse events
    static Mouse::EventHandlerId SubscribeMouse(Mouse::EventHandler handler);
    /// unsubscribe from mouse events
    static void UnsubscribeMouse(Mouse::EventHandlerId id);
    /// subscribe to touchpad events
    static Touchpad::EventHandlerId SubscribeTouchpad(Touchpad::EventHandler handler);
    /// unsubscribe from touchpad events
    static void UnsubscribeTouchpad(Touchpad::EventHandlerId id);
    
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