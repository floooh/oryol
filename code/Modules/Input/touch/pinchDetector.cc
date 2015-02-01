//------------------------------------------------------------------------------
//  pinchDetector.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "pinchDetector.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
void
pinchDetector::reset() {
    if (this->startEvent.type != touchEvent::invalid) {
        this->startEvent = touchEvent();
    }
}

//------------------------------------------------------------------------------
gestureState::Code
pinchDetector::detect(const touchEvent& newEvent) {
    o_assert(newEvent.numTouches > 0);
    
    // check for cancelled event
    if (newEvent.type == touchEvent::cancelled) {
        this->reset();
        return gestureState::none;
    }

    // need 2 touches
    if (newEvent.numTouches != 2) {
        this->reset();
        return gestureState::none;
    }

    // check if touch identifiers are unchanged (number of touches and same touch ids)
    if ((this->startEvent.type != touchEvent::invalid) && !this->startEvent.sameTouches(newEvent)) {
        this->reset();
        return gestureState::none;
    }

    // check for gesture start, move and end
    if (newEvent.type == touchEvent::began) {
        this->startEvent = newEvent;
        this->startPosition0 = newEvent.touchPos(newEvent.points[0].identifier);
        this->startPosition1 = newEvent.touchPos(newEvent.points[1].identifier);
        this->position0 = this->startPosition0;
        this->position1 = this->startPosition1;
        return gestureState::start;
    }
    else if (newEvent.type == touchEvent::moved) {
        // cancel if start event is not valid
        if (this->startEvent.type == touchEvent::invalid) {
            return gestureState::none;
        }
        
        this->position0 = newEvent.touchPos(this->startEvent.points[0].identifier);
        this->position1 = newEvent.touchPos(this->startEvent.points[1].identifier);
        return gestureState::move;
    }
    else if (newEvent.type == touchEvent::ended) {
        this->position0 = newEvent.touchPos(this->startEvent.points[0].identifier);
        this->position1 = newEvent.touchPos(this->startEvent.points[1].identifier);
        this->reset();
        return gestureState::end;
    }
    return gestureState::none;
}
    
} // namespace _priv
} // namespace Oryol