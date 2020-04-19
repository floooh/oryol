#pragma once
//------------------------------------------------------------------------------
#include "Core/Containers/StaticArray.h"
#include "Core/Containers/Map.h"
#include "Core/String/StringAtom.h"
#include "Core/String/String.h"
#include "glm/vec2.hpp"
#include <functional>

namespace Oryol {

//------------------------------------------------------------------------------
/**
    @class Oryol::MouseButton
    @brief mouse buttons
*/
struct MouseButton {
    enum Code {
        Left = 0,
        Middle,
        Right,
        
        NumMouseButtons,
        InvalidMouseButton,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PointerLockMode
    @brief pointer lock modes
*/
struct PointerLockMode {
    enum Code {
        DontCare,
        Enable,
        Disable,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GamepadButton
    @brief gamepad buttons
*/
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

//------------------------------------------------------------------------------
/**
    @class Oryol::GamepadAxis
    @brief gamepad axes
*/
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

//------------------------------------------------------------------------------
/**
    @class Oryol::GamepadMapping
    @brief map raw gamepad buttons and axes to 'semantic' buttons and axes
*/
class GamepadMapping {
public:
    /// lookup raw button index by 'semantic' button
    StaticArray<int, GamepadButton::NumButtons> Buttons;
    /// axis mapping also contains scale/bias values
    struct AxisMapping {
        int Axis = InvalidIndex;
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

//------------------------------------------------------------------------------
/**
    @class Oryol::InputSetup
    @ingroup Input
    @brief configure the Input module
*/
class InputSetup {
public:
    /// initial set of gamepad mappings, use id="__default" to override default mapping
    Map<StringAtom, GamepadMapping> GamepadMappings;
    /// enable/disable single-tap touch gesture
    bool TapEnabled = true;
    /// enable/disable double-tap touch gesture
    bool DoubleTapEnabled = true;
    /// enable/disable pinch touch gesture
    bool PinchEnabled = true;
    /// enable/disable pan touch gesture
    bool PanEnabled = true;
    /// accelerometer enabled
    bool AccelerometerEnabled = true;
    /// gyrometer enabled
    bool GyrometerEnabled = true;
    /// name of HTML5 canvas element with the input focus
    String HtmlElement = "#canvas";
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Key
    @ingroup Input
    @brief input key codes
*/
class Key {
public:
    /// key code enum, these are ripped from the GLFW header
    enum Code {
        // NOTE: don't change order or add keys without also
        // updating the string mapping table in Key.cc!
        InvalidKey = 0,
    
        Space,
        Apostrophe,
        Comma,
        Minus,
        Period,
        Slash,
        N0,
        N1,
        N2,
        N3,
        N4,
        N5,
        N6,
        N7,
        N8,
        N9,
        Semicolon,
        Equal,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        LeftBracket,
        BackSlash,
        RightBracket,
        GraveAccent,
        World1,
        World2,
        Escape,
        Enter,
        Tab,
        BackSpace,
        Insert,
        Delete,
        Right,
        Left,
        Down,
        Up,
        PageUp,
        PageDown,
        Home,
        End,
        CapsLock,
        ScrollLock,
        NumLock,
        PrintScreen,
        Pause,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25,
        Num0,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Num9,
        NumDecimal,
        NumDivide,
        NumMultiply,
        NumSubtract,
        NumAdd,
        NumEnter,
        NumEqual,
        LeftShift,
        LeftControl,
        LeftAlt,
        LeftSuper,
        RightShift,
        RightControl,
        RightAlt,
        RightSuper,
        Menu,

        NumKeys,
    };
    
    /// convert to string
    static const char* ToString(Key::Code key);
private:
    static const char* keyTable[NumKeys];
};

//------------------------------------------------------------------------------
/**
    @class Oryol::InputEvent
    @brief common input event class
*/
class InputEvent {
public:
    /// input event types
    enum Type {
        KeyDown = 0,
        KeyUp,
        KeyRepeat,
        WChar,
        MouseMove,
        MouseButtonDown,
        MouseButtonUp,
        MouseScrolling,
        TouchTapped,
        TouchDoubleTapped,
        TouchPanningStarted,
        TouchPanning,
        TouchPanningEnded,
        TouchPanningCancelled,
        TouchPinchingStarted,
        TouchPinching,
        TouchPinchingEnded,
        TouchPinchingCancelled,

        NumTypes,
        InvalidType
    };

    /// type of this event
    enum Type Type = InvalidType;
    /// key code (if Type is KeyDown, KeyUp, KeyRepeat
    Key::Code KeyCode = Key::InvalidKey;
    /// ASCII code (if Type is WChar)
    wchar_t WCharCode = 0;
    /// mouse movement (if type is MouseMove)
    glm::vec2 Movement;
    /// mouse position (if type is MouseMove)
    glm::vec2 Position;
    /// mouse scrolling (if type is MouseScroll)
    glm::vec2 Scrolling;
    /// mouse button (if type is MouseButtonDown, MouseButtonUp)
    MouseButton::Code Button = MouseButton::InvalidMouseButton;
    /// touch positions
    StaticArray<glm::vec2,2> TouchPosition;
    /// touch start positions
    StaticArray<glm::vec2,2> TouchStartPosition;
    /// touch movement
    StaticArray<glm::vec2,2> TouchMovement;

    InputEvent() { };
    explicit InputEvent(enum Type t, Key::Code k) : Type(t), KeyCode(k) { };
    explicit InputEvent(enum Type t, wchar_t c) : Type(t), WCharCode(c) { };
    explicit InputEvent(enum Type t, const glm::vec2& mov, const glm::vec2& pos) : Type(t), Movement(mov), Position(pos) { };
    explicit InputEvent(enum Type t, MouseButton::Code b) : Type(t), Button(b) { };
    explicit InputEvent(enum Type t, const glm::vec2& scroll) : Type(t), Scrolling(scroll) { };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::inputConfig
    @brief various input system constants
*/
namespace _priv {
struct inputDefs {
    static const int maxNumGamepads = 4;
    static const int maxNumRawButtons = 32;
    static const int maxNumRawAxes = 16;
    typedef std::function<void(const InputEvent&)> inputEventCallback;
    typedef std::function<PointerLockMode::Code(const InputEvent&)> pointerLockCallback;
    typedef int callbackId;
};
} // namespace _priv

} // namespace Oryol
