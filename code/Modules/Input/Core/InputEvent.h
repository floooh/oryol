#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::InputEvent
    @brief common input event class
*/
#include "Core/Types.h"
#include "Input/Core/InputEnums.h"
#include "Input/Core/Key.h"
#include "glm/vec2.hpp"
#include "Core/Containers/StaticArray.h"

namespace Oryol {

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
        MouseScroll,
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
    glm::vec2 MouseMovement;
    /// mouse position (if type is MouseMove)
    glm::vec2 MousePosition;
    /// mouse scrolling (if type is MouseScroll)
    glm::vec2 MouseScrollMovement;
    /// mouse button (if type is MouseButtonDown, MouseButtonUp)
    MouseButton::Code MouseButton = MouseButton::InvalidMouseButton;
    /// touch positions
    StaticArray<glm::vec2,2> TouchPosition;
    /// touch start positions
    StaticArray<glm::vec2,2> TouchStartPosition;
    /// touch movement
    StaticArray<glm::vec2,2> TouchMovement;

    InputEvent() { };
    explicit InputEvent(enum Type t, Key::Code k) : Type(t), KeyCode(k) { };
    explicit InputEvent(enum Type t, wchar_t c) : Type(t), WCharCode(c) { };
    explicit InputEvent(enum Type t, const glm::vec2& mov, const glm::vec2& pos) : Type(t), MouseMovement(mov), MousePosition(pos) { };
    explicit InputEvent(enum Type t, MouseButton::Code b) : Type(t), MouseButton(b) { };
    explicit InputEvent(enum Type t, const glm::vec2& scroll) : Type(t), MouseScrollMovement(scroll) { };
};

} // namespace Oryol
