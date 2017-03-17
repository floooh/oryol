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
    o_assert(((0 == btn % 2) || (1 == btn)) && (btn <= GamepadGizmo::MaxGamepadGizmoCode));
    this->down |= btn;
    this->pressed |= btn;
}

//------------------------------------------------------------------------------
void
gamepadDevice::onButtonUp(GamepadGizmo::Code btn) {
    o_assert(((0 == btn % 2) || (1 == btn)) && (btn <= GamepadGizmo::MaxGamepadGizmoCode));
    this->up |= btn;
    this->pressed &= ~btn;
}

//------------------------------------------------------------------------------
void
gamepadDevice::onTriggerValue(GamepadGizmo::Value trigger, float value) {
    o_assert_range_dbg(trigger, GamepadGizmo::NumGamepadGizmoValues);
    this->values[trigger].x = value;
}

//------------------------------------------------------------------------------
void
gamepadDevice::onStickPos(GamepadGizmo::Value stick, const glm::vec2& pos) {
    o_assert_range_dbg(stick, GamepadGizmo::NumGamepadGizmoValues);
    this->values[stick] = pos;
}

//------------------------------------------------------------------------------
void
gamepadDevice::reset() {
    this->down = 0;
    this->up = 0;
    for (auto& val : this->values) {
        val.x = 0.0f; val.y = 0.0f;
    }
}

} // namespace _priv
} // namespace Oryol
