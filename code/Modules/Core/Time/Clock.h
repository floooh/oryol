#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Clock
    @ingroup Core
    @brief high-resolution time source
    
    The most important method of Clock is Now() which returns the 
    current point in time. The time values returned by Clock have
    no relation to the "wall-clock-time".
*/
#include "Core/Time/TimePoint.h"

namespace Oryol {
    
class Clock {
public:
    /// get current point in time
    static TimePoint Now();
    /// get duration between Now and another TimePoint
    static Duration Since(const TimePoint& t);
    /// get duration between Now and TimePoint in the past, and set TimePoint to Now
    static Duration LapTime(TimePoint& inOutTimepoint);
};

//------------------------------------------------------------------------------
inline Duration
Clock::Since(const TimePoint& t) {
    return Now().Since(t);
}

//------------------------------------------------------------------------------
inline Duration
Clock::LapTime(TimePoint& inOutTimepoint) {
    TimePoint now = Clock::Now();
    Duration dur = now.Since(inOutTimepoint);
    inOutTimepoint = now;
    return dur;
}

} // namespace Oryol