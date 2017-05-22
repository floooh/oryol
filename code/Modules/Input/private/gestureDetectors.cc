//------------------------------------------------------------------------------
//  gestureDetectors.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "gestureDetectors.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
const touchEvent::point*
touchEvent::findPoint(uintptr_t touchId) const {
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
touchEvent::touchPos(uintptr_t touchId) const {
    const point* p = this->findPoint(touchId);
    o_assert_dbg(p);
    return p->pos;
}

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
panDetector::distLess(const glm::vec2& pos0, const glm::vec2& pos1, int dist) const {
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
    if ((newEvent.type != touchEvent::ended) && (newEvent.numTouches != 2)) {
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
        if (this->startEvent.type == touchEvent::invalid) {
            return gestureState::none;
        }
        this->position0 = newEvent.touchPos(this->startEvent.points[0].identifier);
        this->position1 = newEvent.touchPos(this->startEvent.points[1].identifier);
        this->reset();
        return gestureState::end;
    }
    return gestureState::none;
}

//------------------------------------------------------------------------------
void
tapDetector::reset() {
    if (this->tapEvent.type != touchEvent::invalid) {
        this->tapEvent = touchEvent();
    }
    this->tapCount = 0;
}

//------------------------------------------------------------------------------
bool
tapDetector::withinTimeout(const touchEvent& newEvent, const touchEvent& oldEvent, int timeOutMs) const {
    return (newEvent.time - oldEvent.time).AsMilliSeconds() < timeOutMs;
}

//------------------------------------------------------------------------------
bool
tapDetector::withinDistance(const touchEvent& newEvent, const touchEvent& oldEvent, int maxDist) const {

    // FIXME: take device DPI into account

    float dx = newEvent.points[0].pos.x - oldEvent.points[0].pos.x;
    float dy = newEvent.points[0].pos.y - oldEvent.points[0].pos.y;
    return (dx * dx + dy * dy) < (maxDist * maxDist);
}

//------------------------------------------------------------------------------
gestureState::Code
tapDetector::detect(const touchEvent& newEvent) {
    o_assert_dbg(newEvent.numTouches > 0);
    
    // only look at single-touch events, and stop at cancel event
    if ((newEvent.numTouches > 1) || (newEvent.type == touchEvent::cancelled)) {
        this->reset();
        return gestureState::none;
    }
    
    // store down-event as start of a new potential tap
    if (newEvent.type == touchEvent::began) {
        // if multitap, check if within max distance and tap timeout of
        // last tap, if not, start a new multitap sequence
        if (this->tapCount > 0) {
            if (!this->withinTimeout(newEvent, this->tapEvent, TapTimeout)) {
                this->reset();
            }
            if (!this->withinDistance(newEvent, this->tapEvent, MaxTapDistance)) {
                this->reset();
            }
        }
        this->tapEvent = newEvent;
        return gestureState::none;
    }
    
    // check up-event against start event
    if (newEvent.type == touchEvent::ended) {
        // check time stamp
        if (this->withinTimeout(newEvent, this->tapEvent, TouchTimeout)) {
            if (this->withinDistance(newEvent, this->tapEvent, MaxTouchDistance)) {
                // new tap detected
                this->tapCount++;
                this->tapEvent = newEvent;
                if (this->tapCount == this->numRequiredTaps) {
                    this->reset();
                    this->position = newEvent.points[0].pos;
                    return gestureState::action;
                }
            }
        }
    }
    return gestureState::none;
}

} // namespace _priv
} // namespace Oryol

