//------------------------------------------------------------------------------
//  panDetector.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "panDetector.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
void
panDetector::reset() {
    if (this->startEvent.type != touchEvent::invalid) {
        this->startEvent = touchEvent();
    }
    this->panning = false;
}

//------------------------------------------------------------------------------
bool
panDetector::distLess(const glm::vec2& pos0, const glm::vec2& pos1, int32 dist) const {
    float dx = pos1.x - pos0.x;
    float dy = pos1.y - pos0.y;
    return (dx * dx + dy * dy) < (dist * dist);
}

//------------------------------------------------------------------------------
gestureState::Code
panDetector::detect(const touchEvent& newEvent) {
    o_assert_dbg(newEvent.numTouches > 0);
    
    // check for cancelled event
    if (newEvent.type == touchEvent::cancelled) {
        this->reset();
        return gestureState::none;
    }
    
    // check for required number of touches
    if (newEvent.numTouches != 1) {
        this->reset();
        return gestureState::none;
    }
    
    // check if touch identifiers are unchanged (number of touches and same touch ids)
    if ((this->startEvent.type != touchEvent::invalid) && !this->startEvent.sameTouches(newEvent)) {
        this->reset();
        return gestureState::none;
    }
    
    // check whether to start, move or end panning
    if (newEvent.type == touchEvent::began) {
        this->startEvent = newEvent;
        this->startPosition = newEvent.touchPos(newEvent.points[0].identifier);
        this->position = this->startPosition;
        this->panning = false;
        return gestureState::none;
    }
    else if (newEvent.type == touchEvent::moved) {
        // cancel if start event is not valid
        if (this->startEvent.type == touchEvent::invalid) {
            return gestureState::none;
        }
        
        this->position = newEvent.touchPos(this->startEvent.points[0].identifier);
        if (this->panning) {
            // already panning
            return gestureState::move;
        }
        else {
            // not yet panning, check if min-distance is reached before starting pan,
            // and if yes, start panning
            if (!this->distLess(this->startPosition, this->position, MinMovedDistance)) {
                this->panning = true;
                return gestureState::start;
            }
            else {
                // not yet panning
                return gestureState::none;
            }
        }
    }
    else if (newEvent.type == touchEvent::ended) {
        if (this->panning) {
            this->position = newEvent.touchPos(this->startEvent.points[0].identifier);
            this->reset();
            return gestureState::end;
        }
        else {
            this->reset();
            return gestureState::none;
        }
    }
    return gestureState::none;
}
    
} // namespace _priv
} // namespace Oryol