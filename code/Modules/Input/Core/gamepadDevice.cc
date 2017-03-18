//------------------------------------------------------------------------------
//  gamepadDevice.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "gamepadDevice.h"

namespace Oryol {
namespace _priv {
    
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
