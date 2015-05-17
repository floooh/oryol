//------------------------------------------------------------------------------
//  tapDetector.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "tapDetector.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

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
tapDetector::withinTimeout(const touchEvent& newEvent, const touchEvent& oldEvent, int32 timeOutMs) const {
    return (newEvent.time - oldEvent.time).AsMilliSeconds() < timeOutMs;
}

//------------------------------------------------------------------------------
bool
tapDetector::withinDistance(const touchEvent& newEvent, const touchEvent& oldEvent, int32 maxDist) const {

    // FIXME: take device DPI into account

    float32 dx = newEvent.points[0].pos.x - oldEvent.points[0].pos.x;
    float32 dy = newEvent.points[0].pos.y - oldEvent.points[0].pos.y;
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