#pragma once
//------------------------------------------------------------------------------
/**
    @class Time::Clock
    @brief high-resolution time source
    
    The most important method of Clock is Now() which returns the 
    current point in time. The time values returned by Clock have
    no relation to the "wall-clock-time".
*/
#include "Time/TimePoint.h"
#include "Time/Duration.h"

namespace Oryol {
namespace Time {
    
class Clock {
public:
    /// get current point in time
    static TimePoint Now();
    /// get duration between Now and another TimePoint
    static Duration Since(const TimePoint& t);
};

//------------------------------------------------------------------------------
inline Duration
Clock::Since(const TimePoint& t) {
    return Now().Since(t);
}
    
} // namespace Time
} // namespace Oryol