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
    this->axes.Fill(0.0f);
}

} // namespace _priv
} // namespace Oryol
