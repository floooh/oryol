//------------------------------------------------------------------------------
//  Mouse.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Mouse.h"

namespace Oryol {
namespace Input {
    
//------------------------------------------------------------------------------
Mouse::Mouse() :
pos(0.0f, 0.0f),
attached(false),
wheelForward(false),
wheelBackward(false) {
    for (int32 i = 0; i < Button::NumButtons; i++) {
        this->buttonState[i] = 0;
    }
}

//------------------------------------------------------------------------------
void
Mouse::setAttached(bool b) {
    this->attached = b;
}

//------------------------------------------------------------------------------
void
Mouse::onButtonDown(Button btn) {
    o_assert_range_dbg(btn, NumButtons);
    this->buttonState[btn] |= (btnDown | btnPressed);
}

//------------------------------------------------------------------------------
void
Mouse::onButtonUp(Button btn) {
    o_assert_range_dbg(btn, NumButtons);
    this->buttonState[btn] &= ~btnPressed;
    this->buttonState[btn] |= btnUp;
}

//------------------------------------------------------------------------------
void
Mouse::onPos(const glm::vec2& p) {
    this->pos = p;
}

//------------------------------------------------------------------------------
void
Mouse::onWheelForward() {
    this->wheelForward = true;
}

//------------------------------------------------------------------------------
void
Mouse::onWheelBackward() {
    this->wheelBackward = true;
}

//------------------------------------------------------------------------------
void
Mouse::reset() {
    for (int32 i = 0; i < NumButtons; i++) {
        this->buttonState[i] &= ~(btnDown | btnUp);
    }
    this->wheelForward = false;
    this->wheelBackward = false;
}

} // namespace Input
} // namespace Oryol