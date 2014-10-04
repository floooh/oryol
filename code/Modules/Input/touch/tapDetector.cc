//------------------------------------------------------------------------------
//  tapDetector.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "tapDetector.h"
#include "Core/Assert.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
tapDetector::tapDetector() :
tapCount(0) {
    // empty
}

//------------------------------------------------------------------------------
void
tapDetector::reset() {
    this->tapEvent = touch();
    this->position = glm::vec2(0.0f, 0.0f);
    this->tapCount = 0;
}

//------------------------------------------------------------------------------
bool
tapDetector::withinTimeout(const touch& newEvent, const touch& oldEvent, int32 timeOutMs) const {
    return (newEvent.time - oldEvent.time).AsMilliSeconds() < timeOutMs;
}

//------------------------------------------------------------------------------
bool
tapDetector::withinDistance(const touch& newEvent, const touch& oldEvent, float maxDist) const {

    // FIXME: take device DPI into account

    float32 dx = newEvent.points[0].x - oldEvent.points[0].x;
    float32 dy = newEvent.points[0].y - oldEvent.points[0].y;
    return (dx * dx + dy * dy) < (maxDist * maxDist);
}

//------------------------------------------------------------------------------
gestureState::Code
tapDetector::detect(const touch& newEvent) {
    o_assert_dbg(newEvent.numTouches > 0);
    
    // only look at single-touch events, and stop at cancel event
    if ((newEvent.numTouches > 1) || (newEvent.type == touch::cancelled)) {
        this->reset();
        return gestureState::none;
    }
    
    // store down-event as start of a new potential tap
    if (newEvent.type == touch::began) {
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
    if (newEvent.type == touch::ended) {
        // check time stamp
        if (this->withinTimeout(newEvent, this->tapEvent, TouchTimeout)) {
            if (this->withinDistance(newEvent, this->tapEvent, MaxTouchDistance)) {
                // new tap detected
                this->tapCount++;
                this->tapEvent = newEvent;
                if (this->tapCount == this->numRequiredTaps) {
                    this->reset();
                    this->position.x = newEvent.points[0].x;
                    this->position.y = newEvent.points[0].y;
                    return gestureState::action;
                }
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