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
gamepadDevice::reset() {
    this->down = 0;
    this->up = 0;
    for (auto& val : this->values) {
        val.x = 0.0f; val.y = 0.0f;
    }
}

} // namespace _priv
} // namespace Oryol
