#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::tapDetector
    @brief simple tap-gesture recognizer
*/
#include "Input/touch/touch.h"
#include "Input/touch/gestureState.h"
#include "glm/vec2.hpp"

namespace Oryol {
namespace _priv {
    
class tapDetector {
public:
    /// timeout for touch in milliseconds
    static const int32 TouchTimeout = 250;
    /// timeout between multiple taps
    static const int32 TapTimeout = 750;
    /// max pixel distance from tap start
    static const int32 MaxTouchDistance = 20;
    /// max pixel distance between taps
    static const int32 MaxTapDistance = 30;
    
    /// constructor
    tapDetector();
    /// feed new touch event and return detected state (if any)
    gestureState::Code detect(const touch& touchEvent);
    /// reset the detector
    void reset();
    /// get current tap position
    const glm::vec2& pos() const;
    /// number of required taps
    int32 numRequiredTaps = 1;
    
private:
    /// check if 2 touch events are within the tap max distance
    bool withinDistance(const touch& newEvent, const touch& oldEvent, float maxDist) const;
    /// check if 2 touch events are within a timeout
    bool withinTimeout(const touch& newEvent, const touch& oldEvent, int32 timeOutMs) const;

    touch tapEvent;
    glm::vec2 position;
    int32 tapCount;
};
    
} // namespace _priv
} // namespace Oryol