//------------------------------------------------------------------------------
//  Gamepad.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Gamepad.h"

namespace Oryol {
namespace Input {
    
//------------------------------------------------------------------------------
Gamepad::Gamepad() :
down(0),
up(0),
pressed(0),
attached(false) {
    // empty
}

//------------------------------------------------------------------------------
void
Gamepad::onButtonDown(Element btn) {
    o_assert_range_dbg(btn, NumElements);
    this->down |= (1<<btn);
    this->pressed |= (1<<btn);
}

//------------------------------------------------------------------------------
void
Gamepad::onButtonUp(Element btn) {
    o_assert_range_dbg(btn, NumElements);
    this->up |= (1<<btn);
    this->pressed &= ~(1<<btn);
}

//------------------------------------------------------------------------------
void
Gamepad::onTriggerValue(Element trigger, float32 value) {
    o_assert_range_dbg(trigger, NumElements);
    this->values[trigger].x = value;
}

//------------------------------------------------------------------------------
void
Gamepad::onStickPos(Element stick, const glm::vec2& pos) {
    o_assert_range_dbg(stick, NumElements);
    this->values[stick] = pos;
}

//------------------------------------------------------------------------------
void
Gamepad::reset() {
    this->down = 0;
    this->up = 0;
}

} // namespace Input
} // namespace Oryol