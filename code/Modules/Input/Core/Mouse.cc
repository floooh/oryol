//------------------------------------------------------------------------------
//  Mouse.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Mouse.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
Mouse::Mouse() :
Attached(false),
Position(0.0f, 0.0f),
Movement(0.0f, 0.0f),
Scroll(0.0f, 0.0f) {
    for (int32 i = 0; i < Button::NumButtons; i++) {
        this->buttonState[i] = 0;
    }
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
Mouse::onPos(const glm::vec2& pos) {
    this->Movement = pos - this->Position;
    this->Position = pos;
}

//------------------------------------------------------------------------------
void
Mouse::reset() {
    for (int32 i = 0; i < NumButtons; i++) {
        this->buttonState[i] &= ~(btnDown | btnUp);
    }
    this->Movement = glm::vec2(0.0f, 0.0f);
    this->Scroll = glm::vec2(0.0f, 0.0f);
}

} // namespace Oryol
