//------------------------------------------------------------------------------
//  gamepadDevice.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "gamepadDevice.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
gamepadDevice::gamepadDevice() :
attached(false),
down(0),
up(0),
pressed(0) {
    // empty
}

//------------------------------------------------------------------------------
void
gamepadDevice::onButtonDown(GamepadGizmo::Code btn) {
    o_assert_range_dbg(btn, GamepadGizmo::NumGamepadGizmos);
    this->down |= (1<<btn);
    this->pressed |= (1<<btn);
}

//------------------------------------------------------------------------------
void
gamepadDevice::onButtonUp(GamepadGizmo::Code btn) {
    o_assert_range_dbg(btn, GamepadGizmo::NumGamepadGizmos);
    this->up |= (1<<btn);
    this->pressed &= ~(1<<btn);
}

//------------------------------------------------------------------------------
void
gamepadDevice::onTriggerValue(GamepadGizmo::Code trigger, float value) {
    o_assert_range_dbg(trigger, GamepadGizmo::NumGamepadGizmos);
    this->values[trigger].x = value;
}

//------------------------------------------------------------------------------
void
gamepadDevice::onStickPos(GamepadGizmo::Code stick, const glm::vec2& pos) {
    o_assert_range_dbg(stick, GamepadGizmo::NumGamepadGizmos);
    this->values[stick] = pos;
}

//------------------------------------------------------------------------------
void
gamepadDevice::reset() {
    this->down = 0;
    this->up = 0;
}

} // namespace _priv
} // namespace Oryol
