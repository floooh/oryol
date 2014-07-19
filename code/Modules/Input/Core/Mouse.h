#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Input::Mouse
    @brief access to mouse state
*/
#include "Core/Assert.h"
#include "glm/vec2.hpp"

namespace Oryol {
namespace Input {
    
class Mouse {
public:
    /// constructor
    Mouse();
    
    /// mouse buttons
    enum Button {
        InvalidButton = 0,
        
        LMB,
        MMB,
        RMB,
        
        NumButtons,
    };
    
    /// return true if mouse is valid/attached
    bool Attached() const;
    /// test if mouse button is pressed
    bool Pressed(Button btn) const;
    /// test if mouse button was pressed down last frame
    bool Down(Button btn) const;
    /// test if mouse button was release last frame
    bool Up(Button btn) const;
    /// test if wheel forward was triggered
    bool WheelForward() const;
    /// test if wheel backward was triggered
    bool WheelBackward() const;
    /// get mouse position (0.0 .. 1.0)
    const glm::vec2& Pos() const;
    
    /// set mouse attached state
    void setAttached(bool b);
    /// call when mouse button down event happens
    void onButtonDown(Button btn);
    /// call when mouse button up event happens
    void onButtonUp(Button btn);
    /// call when wheel-forward was triggered
    void onWheelForward();
    /// call when wheel-backward was triggered
    void onWheelBackward();
    /// set mouse position (0.0 .. 1.0)
    void onPos(const glm::vec2& p);
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
    bool attached;
    bool wheelForward;
    bool wheelBackward;
};

//------------------------------------------------------------------------------
inline bool
Mouse::Attached() const {
    return this->attached;
}

//------------------------------------------------------------------------------
inline bool
Mouse::Pressed(Button btn) const {
    o_assert_range_dbg(btn, NumButtons);
    return this->buttonState[btn] & btnPressed;
}

//------------------------------------------------------------------------------
inline bool
Mouse::Down(Button btn) const {
    o_assert_range_dbg(btn, NumButtons);
    return this->buttonState[btn] & btnDown;
}

//------------------------------------------------------------------------------
inline bool
Mouse::Up(Button btn) const {
    o_assert_range_dbg(btn, NumButtons);
    return this->buttonState[btn] & btnUp;
}

//------------------------------------------------------------------------------
inline const glm::vec2&
Mouse::Pos() const {
    return this->pos;
}

//------------------------------------------------------------------------------
inline bool
Mouse::WheelForward() const {
    return this->wheelForward;
}

//------------------------------------------------------------------------------
inline bool
Mouse::WheelBackward() const {
    return this->wheelBackward;
}

} // namespace Input
} // namespace Oryol