//------------------------------------------------------------------------------
//  Touchpad.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Input/Core/Touchpad.h"

namespace Oryol {
namespace Input {
    
//------------------------------------------------------------------------------
Touchpad::Touchpad() :
attached(false) {
    // empty
}

//------------------------------------------------------------------------------
bool
Touchpad::Attached() const {
    return this->attached;
}

//------------------------------------------------------------------------------
void
Touchpad::setAttached(bool b) {
    this->attached = b;
}

//------------------------------------------------------------------------------
void
Touchpad::reset() {
    // FIXME!
}

} // namespace Input
} // namespace Oryol