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
move(0.0f, 0.0f),
scroll(0.0f, 0.0f),
attached(false) {
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
    this->move = p - this->pos;
    this->pos = p;
}

//------------------------------------------------------------------------------
void
Mouse::onMove(const glm::vec2& m) {
    this->move = m;
}

//------------------------------------------------------------------------------
void
Mouse::onScroll(const glm::vec2& p) {
    this->scroll = p;
}

//------------------------------------------------------------------------------
void
Mouse::reset() {
    for (int32 i = 0; i < NumButtons; i++) {
        this->buttonState[i] &= ~(btnDown | btnUp);
    }
    this->move = glm::vec2(0.0f, 0.0f);
    this->scroll = glm::vec2(0.0f, 0.0f);
}

} // namespace Input
} // namespace Oryol
