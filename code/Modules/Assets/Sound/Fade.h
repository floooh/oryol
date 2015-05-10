#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Fade
    @ingroup Assets
    @brief fade-in/out modulator for sound sample generation
    
    FIXME: this should give more options then just linear fading
    (e.g. http://easings.net/ )
*/
#include "Core/Types.h"

namespace Oryol {

class Fade {
public:
    /// initialize with effect duration, fade-in and fade-out in seconds
    Fade(float32 bufferDuration, float32 fadeInDuration, float32 fadeOutDuration);
    /// get curren modulator (0.0 .. 1.0) at time position
    float32 Value(float32 t) const;
private:
    float32 duration;
    float32 fadeInPos;
    float32 fadeOutPos;
    float32 fadeInMul;
    float32 fadeOutMul;
};

//------------------------------------------------------------------------------
inline
Fade::Fade(float32 bufferDur, float32 fadeInDur, float32 fadeOutDur) {
    o_assert_dbg(bufferDur > 0.0f);
    o_assert_dbg(fadeInDur >= 0.0f);
    o_assert_dbg(fadeOutDur >= 0.0f);
    this->duration = bufferDur;
    this->fadeInPos = fadeInDur;
    this->fadeOutPos = this->duration - fadeOutDur;
    this->fadeInMul = 1.0f / fadeInDur;
    this->fadeOutMul = 1.0f / fadeOutDur;
}

//------------------------------------------------------------------------------
inline float32
Fade::Value(float32 t) const {
    if (t < this->fadeInPos) {
        return t * this->fadeInMul;
    }
    else if (t >= this->fadeOutPos) {
        return 1.0f - ((t - this->fadeOutPos) * this->fadeOutMul);
    }
    else {
        return 1.0f;
    }
}

} // namespace Oryol