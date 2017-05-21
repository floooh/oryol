#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TimePoint
    @ingroup Core
    @brief a point in time
    
    A TimePoint holds a specific point in time returned by the high-resolution
    Clock. There is no relation to the wall-clock time, the TimePoint '0'
    has no specific meaning. The only meaningful operations on TimePoints
    is comparison to other TimePoints, subtracting TimePoints to gain
    a Duration, or adding a Duration to a TimePoint to get another TimePoint.
    
    @see Duration
*/
#include "Core/Time/Duration.h"

namespace Oryol {
    
class TimePoint {
public:
    /// default constructor
    TimePoint();
    /// copy constructor
    TimePoint(const TimePoint& rhs);
    /// create from a raw time value
    TimePoint(int64_t raw);
    
    /// assignment
    void operator=(const TimePoint& rhs);
    /// add a duration
    void operator+=(const Duration& rhs);
    /// subtract a duration
    void operator-=(const Duration& rhs);
    /// equality
    bool operator==(const TimePoint& rhs) const;
    /// inequality
    bool operator!=(const TimePoint& rhs) const;
    /// less-then
    bool operator<(const TimePoint& rhs) const;
    /// greater-then
    bool operator>(const TimePoint& rhs) const;
    /// less-or-equal
    bool operator<=(const TimePoint& rhs) const;
    /// greater-or-equal
    bool operator>=(const TimePoint& rhs) const;
    
    /// return duration since another point in time
    Duration Since(const TimePoint& rhs) const;
    /// get the raw time value
    int64_t getRaw() const;
    
private:
    int64_t val;
};

//------------------------------------------------------------------------------
inline TimePoint operator+(TimePoint t, const Duration& d) {
    t += d;
    return t;
}

//------------------------------------------------------------------------------
inline TimePoint operator+(const Duration& d, TimePoint t) {
    t += d;
    return t;
}

//------------------------------------------------------------------------------
inline TimePoint operator-(TimePoint t, const Duration& d) {
    t -= d;
    return t;
}

//------------------------------------------------------------------------------
inline Duration operator-(const TimePoint& a, const TimePoint& b) {
    return a.Since(b);
}

//------------------------------------------------------------------------------
inline
TimePoint::TimePoint() :
val(0) {
    // empty
}

//------------------------------------------------------------------------------
inline
TimePoint::TimePoint(const TimePoint& rhs) :
val(rhs.val) {
    // empty
}

//------------------------------------------------------------------------------
inline
TimePoint::TimePoint(int64_t raw) :
val(raw) {
    // empty
}

//------------------------------------------------------------------------------
inline void
TimePoint::operator=(const TimePoint& rhs) {
    this->val = rhs.val;
}

//------------------------------------------------------------------------------
inline void
TimePoint::operator+=(const Duration& rhs) {
    this->val += rhs.val;
}

//------------------------------------------------------------------------------
inline void
TimePoint::operator-=(const Duration& rhs) {
    this->val -= rhs.val;
}

//------------------------------------------------------------------------------
inline bool
TimePoint::operator==(const TimePoint& rhs) const {
    return this->val == rhs.val;
}

//------------------------------------------------------------------------------
inline bool
TimePoint::operator!=(const TimePoint& rhs) const {
    return this->val != rhs.val;
}

//------------------------------------------------------------------------------
inline bool
TimePoint::operator<(const TimePoint& rhs) const {
    return this->val < rhs.val;
}

//------------------------------------------------------------------------------
inline bool
TimePoint::operator>(const TimePoint& rhs) const {
    return this->val > rhs.val;
}

//------------------------------------------------------------------------------
inline bool
TimePoint::operator<=(const TimePoint& rhs) const {
    return this->val <= rhs.val;
}

//------------------------------------------------------------------------------
inline bool
TimePoint::operator>=(const TimePoint& rhs) const {
    return this->val >= rhs.val;
}

//------------------------------------------------------------------------------
inline Duration
TimePoint::Since(const TimePoint& rhs) const {
    return Duration(this->val - rhs.val);
}

//------------------------------------------------------------------------------
inline int64_t
TimePoint::getRaw() const {
    return this->val;
}

} // namespace Oryol
