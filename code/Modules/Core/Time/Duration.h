#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Duration
    @ingroup Core
    @brief a duration (distance between two TimePoints)
    
    Durations are created by subtracting one TimePoint from another. Durations
    can be converted to various time units.

    @see TimePoint
*/
#include "Core/Types.h"

namespace Oryol {
    
class Duration {
public:
    /// default constructor
    Duration();
    /// copy-construct
    Duration(const Duration& rhs);
    /// construct from raw value
    Duration(int64_t raw);
    
    /// assignment
    void operator=(const Duration& rhs);
    /// add a Duration
    void operator+=(const Duration& rhs);
    /// scale a Duration (result will be clamped)
    void operator*=(double s);
    /// subtract a Duration
    void operator-=(const Duration& rhs);
    /// equality
    bool operator==(const Duration& rhs) const;
    /// inequality
    bool operator!=(const Duration& rhs) const;
    /// less-then
    bool operator<(const Duration& rhs) const;
    /// greater-then
    bool operator>(const Duration& rhs) const;
    /// less-or-equal
    bool operator<=(const Duration& rhs) const;
    /// greater-or-equal
    bool operator>=(const Duration& rhs) const;

    /// get duration in seconds
    double AsSeconds() const;
    /// get duration in milliseconds
    double AsMilliSeconds() const;
    /// get duration in microseconds
    double AsMicroSeconds() const;
    /// as microseconds integer ticks
    int64_t AsTicks() const;
    
    /// get duration from seconds
    static Duration FromSeconds(double s);
    /// get duration from seconds
    static Duration FromSeconds(float s);
    /// get duration from milliseconds
    static Duration FromMilliSeconds(double ms);
    /// get duration from milliseconds
    static Duration FromMilliSeconds(float ms);
    /// get duration from microseconds
    static Duration FromMicroSeconds(double us);
    /// get duration from microseconds
    static Duration FromMicroSeconds(float us);

    /// get the raw value
    int64_t getRaw() const;

private:
    friend class TimePoint;
    int64_t val;
};

//------------------------------------------------------------------------------
inline Duration operator+(Duration a, const Duration& b) {
    a += b;
    return a;
}

//------------------------------------------------------------------------------
inline Duration operator-(Duration a, const Duration& b) {
    a -= b;
    return b;
}

//------------------------------------------------------------------------------
inline Duration operator*(Duration a, float s) {
    a *= s;
    return a;
}

//------------------------------------------------------------------------------
inline
Duration::Duration() :
val(0) {
    // empty
}

//------------------------------------------------------------------------------
inline
Duration::Duration(const Duration& rhs) :
val(rhs.val) {
    // empty
}

//------------------------------------------------------------------------------
inline
Duration::Duration(int64_t raw) :
val(raw) {
    // empty
}

//------------------------------------------------------------------------------
inline void
Duration::operator=(const Duration& rhs) {
    this->val = rhs.val;
}

//------------------------------------------------------------------------------
inline void
Duration::operator+=(const Duration& rhs) {
    this->val += rhs.val;
}

//------------------------------------------------------------------------------
inline void
Duration::operator-=(const Duration& rhs) {
    this->val -= rhs.val;
}

//------------------------------------------------------------------------------
inline void
Duration::operator*=(double s) {
    this->val = (int64_t) (double(this->val) * s);
}

//------------------------------------------------------------------------------
inline bool
Duration::operator==(const Duration& rhs) const {
    return this->val == rhs.val;
}

//------------------------------------------------------------------------------
inline bool
Duration::operator!=(const Duration& rhs) const {
    return this->val != rhs.val;
}

//------------------------------------------------------------------------------
inline bool
Duration::operator<(const Duration& rhs) const {
    return this->val < rhs.val;
}

//------------------------------------------------------------------------------
inline bool
Duration::operator>(const Duration& rhs) const {
    return this->val > rhs.val;
}

//------------------------------------------------------------------------------
inline bool
Duration::operator<=(const Duration& rhs) const {
    return this->val <= rhs.val;
}

//------------------------------------------------------------------------------
inline bool
Duration::operator>=(const Duration& rhs) const {
    return this->val >= rhs.val;
}

//------------------------------------------------------------------------------
inline int64_t
Duration::getRaw() const {
    // NOTE: this is not guaranteed to be in nano-seconds!
    return this->val;
}

//------------------------------------------------------------------------------
inline double
Duration::AsSeconds() const {
    return double(this->val) / 1000000.0;
}

//------------------------------------------------------------------------------
inline Duration
Duration::FromSeconds(double s) {
    return Duration(int64_t(s * 1000000.0));
}

//------------------------------------------------------------------------------
inline Duration
Duration::FromSeconds(float s) {
    return Duration(int64_t(s * 1000000.0f));
}

//------------------------------------------------------------------------------
inline double
Duration::AsMilliSeconds() const {
    return double(this->val) / 1000.0;
}

//------------------------------------------------------------------------------
inline Duration
Duration::FromMilliSeconds(double ms) {
    return Duration(int64_t(ms * 1000.0));
}

//------------------------------------------------------------------------------
inline Duration
Duration::FromMilliSeconds(float ms) {
    return Duration(int64_t(ms * 1000.0f));
}

//------------------------------------------------------------------------------
inline double
Duration::AsMicroSeconds() const {
    return double(this->val);
}

//------------------------------------------------------------------------------
inline Duration
Duration::FromMicroSeconds(double us) {
    return Duration(int64_t(us));
}

//------------------------------------------------------------------------------
inline Duration
Duration::FromMicroSeconds(float us) {
    return Duration(int64_t(us));
}

//------------------------------------------------------------------------------
inline int64_t
Duration::AsTicks() const {
    return this->val;
}

} // namespace Oryol