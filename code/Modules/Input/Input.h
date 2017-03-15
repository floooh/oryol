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
    /// input event callback function
    typedef _priv::inputDispatcher::inputEventCallback InputEventCallback;
    /// mouse pointer lock handler type
    typedef _priv::inputDispatcher::pointerLockCallback PointerLockCallback;;
    /// typedef for input event callback id
    typedef _priv::inputDispatcher::callbackId CallbackId;
    /// subscribe to input events
    static CallbackId SubscribeEvents(InputEventCallback cb);
    /// unsubscribe from input events
    static void UnsubscribeEvents(CallbackId id);
    /// set mouse pointer-lock handler, decide here when to activate/deactive pointer-lock
    static void SetPointerLockHandler(PointerLockCallback cb);
    /// clear the mouse pointer-lock handler
    static void ClearPointerLockHandler();

    /// return true if a keyboard is attached
    static bool KeyboardAttached();
    /// test if a key is currently held down
    static bool KeyPressed(Key::Code key);
    /// test if a key was pressed down during the last frame
    static bool KeyDown(Key::Code key);
    /// test if a key was released during the last frame
    static bool KeyUp(Key::Code key);
    /// test if a key repeat happened during the last frame
    static bool KeyRepeat(Key::Code key);
    /// test if any key is pressed
    static bool AnyKeyPressed();
    /// test if any key was pressed down last frame
    static bool AnyKeyDown();
    /// test if any key was released last frame
    static bool AnyKeyUp();
    /// test if any key was repeated last frame
    static bool AnyKeyRepeat();
    /// get captured text
    static const wchar_t* Text();

    /// return true if a mouse is attached
    static bool MouseAttached();
    /// test if mouse button is pressed
    static bool MouseButtonPressed(MouseButton::Code btn);
    /// test if mouse button was pressed down last frame
    static bool MouseButtonDown(MouseButton::Code btn);
    /// test if mouse button was release last frame
    static bool MouseButtonUp(MouseButton::Code btn);
    /// mouse position in pixel coords
    static const glm::vec2& MousePosition();
    /// mouse movement in pixels since last frame
    static const glm::vec2& MouseMovement();
    /// scroll movement (usually provided by mouse wheel)
    static const glm::vec2& MouseScroll();

    /// return true if a gamepad is attached
    static bool GamepadAttached(int gamepadIndex);
    /// test if a gamepad button is currently pressed
    static bool GamepadButtonPressed(int gamepadIndex, GamepadGizmo::Code btn);
    /// test if gamepad button was pressed-down this frame
    static bool GamepadButtonDown(int gamepadIndex, GamepadGizmo::Code btn);
    /// test if gamepad button was released this frame
    static bool GamepadButtonUp(int gamepadIndex, GamepadGizmo::Code btn);
    /// get gamepad trigger position (0.0 .. 1.0)
    static float GamepadTriggerValue(int gamepadIndex, GamepadGizmo::Value trigger);
    /// get stick position (-1.0 .. +1.0)
    static const glm::vec2& GamepadStickPos(int gamepadIndex, GamepadGizmo::Value stick);

    /// return true if a touchpad is attached
    static bool TouchpadAttached();
    /// touchpad 'raw touch'?
    static bool TouchTouched();
    /// touchpad single-tapped?
    static bool TouchTapped();
    /// touchpad double-tapped?
    static bool TouchDoubleTapped();
    /// touchpad started panning?
    static bool TouchPanningStarted();
    /// touchpad panning gesture underway?
    static bool TouchPanning();
    /// touchpad ended panning?
    static bool TouchPanningEnded();
    /// touchpad started pinching?
    static bool TouchPinchingStarted();
    /// touchpad pinching underway?
    static bool TouchPinching();
    /// touchpad ended pinching?
    static bool TouchPinchingEnded();
    /// touchpad current position (touchIndex: 0 for tap/pan, 0..1 for pinch)
    static const glm::vec2& TouchPosition(int touchIndex);
    /// touchpad current movement vector (touchIndex: 0 for tap/pan, 0..1 for pinch)
    static const glm::vec2& TouchMovement(int touchIndex);
    /// touchpad current start position (touchIndex: 0 for tap/pan, 0..1 for pinch)
    static const glm::vec2& TouchStartPosition(int touchIndex);

    /// return true if sensory is attached
    static bool SensorsAttached();
    /// gravity sensor acceleration vector including gravity in m/sec^2
    static const glm::vec3& SensorAcceleration();
    /// get device orientation as yaw=x, pitch=y, roll=z angles
    static const glm::vec3& SensorYawPitchRoll();

private:
    struct _state {
        _priv::inputMgr inputManager;
    };
    static _state* state;
};

} // namespace Oryol
