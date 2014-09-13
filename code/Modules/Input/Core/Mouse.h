#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Mouse
    @ingroup Input
    @brief access to mouse state
*/
#include "Core/Assert.h"
#include "glm/vec2.hpp"

namespace Oryol {
    
class Mouse {
public:
    /// constructor
    Mouse();
    
    /// mouse buttons
    enum Button {
        LMB = 0,
        MMB,
        RMB,
        
        NumButtons,
        InvalidButton,
    };
    
    /// return true if mouse is valid/attached
    bool Attached() const;
    /// test if mouse button is pressed
    bool ButtonPressed(Button btn) const;
    /// test if mouse button was pressed down last frame
    bool ButtonDown(Button btn) const;
    /// test if mouse button was release last frame
    bool ButtonUp(Button btn) const;
    /// get mouse position (unbounded pixel coords)
    const glm::vec2& Position() const;
    /// get mouse movement since last frame
    const glm::vec2& Movement() const;
    /// get scroll movement (usually provided by mouse wheel)
    const glm::vec2& Scroll() const;
    
    /// set mouse attached state
    void setAttached(bool b);
    /// call when mouse button down event happens
    void onButtonDown(Button btn);
    /// call when mouse button up event happens
    void onButtonUp(Button btn);
    /// set mouse position (unbounded pixel coords), also updates movement
    void onPos(const glm::vec2& p);
    /// directly set movement
    void onMove(const glm::vec2& m);
    /// set scroll position
    void onScroll(const glm::vec2& p);
    /// reset the mouse state
    void reset();
    
private:
    enum flags {
        btnDown = (1<<0),
        btnUp = (1<<1),
        btnPressed = (1<<2)
    };
    
    uint8 buttonState[Button::NumButtons];
    glm::vec2 pos;
    glm::vec2 move;
    glm::vec2 scroll;
    bool attached;
};

//------------------------------------------------------------------------------
inline bool
Mouse::Attached() const {
    return this->attached;
}

//------------------------------------------------------------------------------
inline bool
Mouse::ButtonPressed(Button btn) const {
    o_assert_range_dbg(btn, NumButtons);
    return this->buttonState[btn] & btnPressed;
}

//------------------------------------------------------------------------------
inline bool
Mouse::ButtonDown(Button btn) const {
    o_assert_range_dbg(btn, NumButtons);
    return this->buttonState[btn] & btnDown;
}

//------------------------------------------------------------------------------
inline bool
Mouse::ButtonUp(Button btn) const {
    o_assert_range_dbg(btn, NumButtons);
    return this->buttonState[btn] & btnUp;
}

//------------------------------------------------------------------------------
inline const glm::vec2&
Mouse::Position() const {
    return this->pos;
}

//------------------------------------------------------------------------------
inline const glm::vec2&
Mouse::Movement() const {
    return this->move;
}

//------------------------------------------------------------------------------
inline const glm::vec2&
Mouse::Scroll() const {
    return this->scroll;
}

} // namespace Oryol
