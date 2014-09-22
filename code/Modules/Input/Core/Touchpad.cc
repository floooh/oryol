//------------------------------------------------------------------------------
//  Touchpad.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assert.h"
#include "Input/Core/Touchpad.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
Touchpad::Touchpad() :
Attached(false),
Tapped(false),
DoubleTapped(false),
Pinching(false),
Panning(false) {
    // empty
}

//------------------------------------------------------------------------------
void
Touchpad::reset() {
    this->Tapped = false;
    this->DoubleTapped = false;
    this->Pinching = false;
    this->Panning = false;
    for (int i = 0; i < MaxNumTouches; i++) {
        this->mov[i] = glm::vec2(0.0f, 0.0f);
    }
}

//------------------------------------------------------------------------------
void
Touchpad::onPos(int32 touchIndex, const glm::vec2& p) {
    o_assert_range_dbg(touchIndex, MaxNumTouches);
    this->pos[touchIndex] = p;
}

//------------------------------------------------------------------------------
void
Touchpad::onPosMov(int32 touchIndex, const glm::vec2& p) {
    o_assert_range_dbg(touchIndex, MaxNumTouches);
    this->mov[touchIndex] = p - this->pos[touchIndex];
    this->pos[touchIndex] = p;
}

} // namespace Oryol