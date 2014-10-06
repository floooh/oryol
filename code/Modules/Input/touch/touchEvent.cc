//------------------------------------------------------------------------------
//  touch.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assert.h"
#include "touchEvent.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
const touchEvent::point*
touchEvent::findPoint(uintptr touchId) const {
    for (int i = 0; i < numTouches; i++) {
        if (this->points[i].identifier == touchId) {
            return &(this->points[i]);
        }
    }
    return nullptr;
}

//------------------------------------------------------------------------------
bool
touchEvent::sameTouches(const touchEvent& other) const {
    if (other.numTouches == this->numTouches) {
        for (int i = 0; i < this->numTouches; i++) {
            if (nullptr == this->findPoint(other.points[i].identifier)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
const glm::vec2&
touchEvent::touchPos(uintptr touchId) const {
    const point* p = this->findPoint(touchId);
    o_assert_dbg(p);
    return p->pos;
}

} // namespace _priv
} // namespace Oryol