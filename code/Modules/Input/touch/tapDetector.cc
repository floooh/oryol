//------------------------------------------------------------------------------
//  tapDetector.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "tapDetector.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
gestureState::Code
tapDetector::detect(const touch& touchEvent) {
    
    // only look at single-touch events, and stop at cancel event
    if (touchEvent.numTouches > 1) {
        return gestureState::none;
    }
    
    // store down-event as start of a new potential tap
    if (touchEvent.type == touch::began) {
        this->startEvent = touchEvent;
        return gestureState::none;
    }
    
    // check up-event against start event
    if (touchEvent.type == touch::ended) {
        // check time stamp
        if ((touchEvent.time - this->startEvent.time).AsMilliSeconds() < Timeout) {
            // check distance to start touch
            float32 dx = touchEvent.points[0].x - this->startEvent.points[0].x;
            float32 dy = touchEvent.points[0].y - this->startEvent.points[0].y;
            if ((dx * dx + dy * dy) < MaxDistance * MaxDistance) {
                this->position.x = touchEvent.points[0].x;
                this->position.y = touchEvent.points[0].y;
                return gestureState::action;
            }
        }
    }
    return gestureState::none;
}

//------------------------------------------------------------------------------
const glm::vec2&
tapDetector::pos() const {
    return this->position;
}

} // namespace _priv
} // namespace Oryol