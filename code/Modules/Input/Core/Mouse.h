#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Mouse
    @ingroup Input
    @brief access to mouse state
*/
#include "Core/Assertion.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/Map.h"
#include "glm/vec2.hpp"
#include <functional>

namespace Oryol {
    
class Mouse {
public:
    /// mouse buttons
    enum Button {
        LMB = 0,
        MMB,
        RMB,
        
        NumButtons,
        InvalidButton,
    };

    /// mouse event for event-driven input handling (vs polling)
    struct Event {
        /// event type
        enum Type {
            Move = 0,
            ButtonDown,
            ButtonUp,
            Scroll,

            NumTypes,
            InvalidType
        } Type;

        /// mouse movement (if type is Move)
        glm::vec2 Movement;
        /// mouse position (if type is Move)
        glm::vec2 Position;
        /// mouse scrolling (if type is Scroll)
        glm::vec2 ScrollMovement;
        /// mouse button (if type is ButtonDown, ButtonUp)
        enum Button Button;

        /// default constructor
        Event() : Type(InvalidType), Button(InvalidButton) { };
        /// movement constructor
        Event(enum Type t, const glm::vec2& move, const glm::vec2& pos) :
            Type(t), Movement(move), Position(pos) { };
        /// button constructor
        Event(enum Type t, enum Button btn) : Type(t), Button(btn) { };
        /// scroll constructor
        Event(enum Type t, glm::vec2 scroll) : Type(t), ScrollMovement(scroll) { };
    };

    /// mouse event handler typedef
    typedef std::function<void(const Event&)> EventHandler;
    
    /// constructor
    Mouse();
    /// mouse attached?
    bool Attached;
    /// test if mouse button is pressed
    bool ButtonPressed(Button btn) const;
    /// test if mouse button was pressed down last frame
    bool ButtonDown(Button btn) const;
    /// test if mouse button was release last frame
    bool ButtonUp(Button btn) const;
    /// mouse position (unbounded pixel coords)
    glm::vec2 Position;
    /// mouse movement since last frame
    glm::vec2 Movement;
    /// scroll movement (usually provided by mouse wheel)
    glm::vec2 Scroll;

    /// subscribe to mouse events
    void subscribe(const StringAtom& id, EventHandler handler);
    /// unsubscribe from mouse events
    void unsubscribe(const StringAtom& id);
    /// call when mouse button down event happens
    void onButtonDown(Button btn);
    /// call when mouse button up event happens
    void onButtonUp(Button btn);
    /// called to update position and computed movement
    void onPosMov(const glm::vec2& pos);
    /// update position and explicit movement 
    void onPosMov(const glm::vec2& pos, const glm::vec2& mov);
    /// called to update movement only
    void onMov(const glm::vec2& mov);
    /// called to update wheel scrolling
    void onScroll(const glm::vec2& scroll);
    /// reset the mouse state
    void reset();
    
private:
    /// notify event handlers
    void notifyEventHandlers(const Event& event);

    enum flags {
        btnDown = (1<<0),
        btnUp = (1<<1),
        btnPressed = (1<<2)
    };
    
    uint8 buttonState[Button::NumButtons];
    Map<StringAtom, EventHandler> eventHandlers;
};

//------------------------------------------------------------------------------
inline bool
Mouse::ButtonPressed(Button btn) const {
    o_assert_range_dbg(btn, NumButtons);
    return 0 != (this->buttonState[btn] & btnPressed);
}

//------------------------------------------------------------------------------
inline bool
Mouse::ButtonDown(Button btn) const {
    o_assert_range_dbg(btn, NumButtons);
    return 0 != (this->buttonState[btn] & btnDown);
}

//------------------------------------------------------------------------------
inline bool
Mouse::ButtonUp(Button btn) const {
    o_assert_range_dbg(btn, NumButtons);
    return 0 != (this->buttonState[btn] & btnUp);
}

} // namespace Oryol
