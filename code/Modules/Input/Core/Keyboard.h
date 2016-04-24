#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Keyboard
    @ingroup Input
    @brief access to keyboard state
*/
#include "Input/Core/Key.h"
#include "Core/Assertion.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/Map.h"
#include <bitset>
#include <functional>

namespace Oryol {

class Keyboard {
public:
    /// keyboard event for event-driven input handling (vs polling)
    struct Event {
        /// event type
        enum Type {
            KeyDown = 0,
            KeyUp,
            KeyRepeat,
            WChar,

            NumTypes,
            InvalidType,

        } Type;
        /// key code (if Type is KeyDown, KeyUp, KeyRepeat
        Key::Code KeyCode;
        /// ASCII code (if Type is WChar)
        wchar_t WCharCode;

        /// default constructor
        Event() : Type(InvalidType), KeyCode(Key::InvalidKey), WCharCode(0) { };
        /// construct from type and key code
        Event(enum Type t, Key::Code k) : Type(t), KeyCode(k), WCharCode(0) { };
        /// construct from type and wchar code
        Event(enum Type t, wchar_t c) : Type(t), KeyCode(Key::InvalidKey), WCharCode(c) { };
    };

    /// unique event handler id typedef
    typedef unsigned int EventHandlerId;
    /// keyboard event handler typedef
    typedef std::function<void(const Event&)> EventHandler;

    /// constructor
    Keyboard();

    /// keyboard attached?
    bool Attached;
    /// test if key is currently pressed
    bool KeyPressed(Key::Code key) const;
    /// test if key was pressed down last frame
    bool KeyDown(Key::Code key) const;
    /// test if key was released last frame
    bool KeyUp(Key::Code key) const;
    /// test if key-repeat happened
    bool KeyRepeat(Key::Code key) const;
    /// test if any key is pressed
    bool AnyKeyPressed() const;
    /// test if any key was pressed down last frame
    bool AnyKeyDown() const;
    /// test if any key was released last frame
    bool AnyKeyUp() const;
    /// test if any key was repeated last frame
    bool AnyKeyRepeat() const;
    
    /// return true if text capturing is active
    bool IsCapturingText() const;
    /// get current captured text
    const wchar_t* CapturedText() const;

    /// subscribe to keyboard events
    EventHandlerId subscribe(EventHandler handler);
    /// unsubscribe from keyboard events
    void unsubscribe(EventHandlerId id);
    /// call when key down event happens
    void onKeyDown(Key::Code key);
    /// call when key up event happens
    void onKeyUp(Key::Code key);
    /// call when key repeat event happens
    void onKeyRepeat(Key::Code key);
    /// call when character is appended
    void onChar(wchar_t c);
    /// reset the keyboard state
    void reset();
    /// begin text capturing
    void beginCaptureText();
    /// end text capturing
    void endCaptureText();
    /// clear captured text (called in beginCaptureText() and reset())
    void clearCapturedText();

private:
    /// notify event handlers
    void notifyEventHandlers(const Event& event);

    std::bitset<Key::NumKeys> down;
    std::bitset<Key::NumKeys> up;
    std::bitset<Key::NumKeys> pressed;
    std::bitset<Key::NumKeys> repeat;
    static const int MaxNumChars = 128;
    int charIndex;
    wchar_t chars[MaxNumChars + 1];
    bool textCapturing;
    EventHandlerId uniqueIdCounter;
    Map<EventHandlerId, EventHandler> eventHandlers;

};

//------------------------------------------------------------------------------
inline bool
Keyboard::KeyPressed(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->pressed[key];
}

//------------------------------------------------------------------------------
inline bool
Keyboard::KeyDown(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->down[key];
}

//------------------------------------------------------------------------------
inline bool
Keyboard::KeyUp(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->up[key];
}

//------------------------------------------------------------------------------
inline bool
Keyboard::KeyRepeat(Key::Code key) const {
    o_assert_range_dbg(key, Key::NumKeys);
    return this->repeat[key];
}

//------------------------------------------------------------------------------
inline bool
Keyboard::AnyKeyPressed() const {
    return this->pressed.any();
}

//------------------------------------------------------------------------------
inline bool
Keyboard::AnyKeyDown() const {
    return this->down.any();
}

//------------------------------------------------------------------------------
inline bool
Keyboard::AnyKeyUp() const {
    return this->up.any();
}

//------------------------------------------------------------------------------
inline bool
Keyboard::AnyKeyRepeat() const {
    return this->repeat.any();
}

//------------------------------------------------------------------------------
inline bool
Keyboard::IsCapturingText() const {
    return this->textCapturing;
}

//------------------------------------------------------------------------------
inline const wchar_t*
Keyboard::CapturedText() const {
    return this->chars;
}
    
} // namespace Oryol

