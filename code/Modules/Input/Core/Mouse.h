#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Mouse
    @ingroup Input
    @brief access to mouse state
*/
#include "Core/Assertion.h"
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
    
    /// call when mouse button down event happens
    void onButtonDown(Button btn);
    /// call when mouse button up event happens
    void onButtonUp(Button btn);
    /// called to update position and movement
    void onPosMov(const glm::vec2& pos);
    /// reset the mouse state
    void reset();
    
private:
    enum flags {
        btnDown = (1<<0),
        btnUp = (1<<1),
        btnPressed = (1<<2)
    };
    
    uint8 buttonState[Button::NumButtons];
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
