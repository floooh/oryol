#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Freq
    @ingroup Assets
    @brief a frequency sampler for sound effect sample generation
    
    A frequency sampler maintains a normalized sample position with variable
    frequency. The sample position is always modulo 1.0f (>=0.0f and <1.0f).
    The output of a frequency counter is a sample position
    which can be fed into a wave generator.
*/
#include "Core/Types.h"
#include <cmath>

namespace Oryol {

class Freq {
public:
    /// construct with sample duration (default frequency is 440Hz)
    Freq(float32 dt);
    /// construct with sample duration and frequency
    Freq(float32 dt, float32 freq);
    /// get next sample position, and advance
    float32 Step();
    /// get next sample position with variable frequency, and advance
    float32 Step(float32 freq);
private:
    float32 dt;             // duration of one sample in seconds
    float32 step;           // current step size
    float32 pos;            // current sample position
};

//------------------------------------------------------------------------------
inline
Freq::Freq(float32 dt_) {
    o_assert_dbg(dt_ > 0.0f);
    this->dt = dt_;
    this->step = this->dt * 440.0f;
    this->pos = 0.0f;
}

//------------------------------------------------------------------------------
inline
Freq::Freq(float32 dt_, float32 freq) {
    o_assert_dbg(dt_ > 0.0f);
    o_assert_dbg(freq > 0.0f);
    this->dt = dt_;
    this->step = this->dt * freq;
    this->pos = 0.0f;
}

//------------------------------------------------------------------------------
inline float32
Freq::Step() {
    float32 p = this->pos;
    this->pos += this->step;
    if (this->pos >= 1.0f) {
        this->pos = std::fmod(this->pos, 1.0f);
    }
    return p;
}

//------------------------------------------------------------------------------
inline float32
Freq::Step(float32 freq) {
    o_assert_dbg(freq > 0.0f);
    this->step = this->dt * freq;
    float32 p = this->pos;
    this->pos += this->step;
    if (this->pos >= 1.0f) {
        this->pos = std::fmod(this->pos, 1.0f);
    }
    return p;
}

} // namespace Oryol