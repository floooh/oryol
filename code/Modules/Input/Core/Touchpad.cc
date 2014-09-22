//------------------------------------------------------------------------------
//  Touchpad.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Input/Core/Touchpad.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
Touchpad::Touchpad() :
Attached(false),
Tapped(false),
DoubleTapped(false),
Pinching(false),
Panning(false),
Position(0.0f, 0.0f),
Movement(0.0f, 0.0f) {
    // empty
}

//------------------------------------------------------------------------------
void
Touchpad::reset() {
    this->Tapped = false;
    this->DoubleTapped = false;
    this->Pinching = false;
    this->Panning = false;
    this->Movement = glm::vec2(0.0f, 0.0f);
}

} // namespace Oryol