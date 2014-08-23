#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::synth
    @brief audio synthesizing utility functions
    
    FIXME: most of this stuff should eventually go onto the GPU,
    FIXME: before that, at least move the waveform functions into 
    precomputed lookup tables
*/
#include "Core/Types.h"
#include <cmath>
#include <cstdlib>

namespace Oryol {
namespace Synth {
    
class synth {
public:
    /// standard sample rate
    static const int32 SampleRate = 22050;
    /// byte size of a single sample
    static const int32 SampleSize = 2;
    /// number of samples in one streaming buffer
    static const int32 BufferNumSamples = 1 * 1024;
    /// byte size of one streaming buffer
    static const int32 BufferSize = SampleSize * BufferNumSamples;
    /// number of voices
    static const int32 NumVoices = 4;
    /// number of tracks per voice
    static const int32 NumTracks = 4;
    
    /// convert time in seconds to sample ticks
    static int32 TimeToTicks(float32 seconds, int32 freq);
    /// convert sample ticks to time in seconds
    static float32 TicksToTime(int32 ticks, int32 freq);
    /// get sample duration in seconds
    static float32 SampleDuration(int32 freq);
    /// get a sine wave sample (t: 0..1), return value between -1 and 1
    static float32 Sine(float32 t);
    /// get a triangle wave sample (t is 0..1), return value between -1 and 1
    static float32 Triangle(float32 t);
    /// get a saw-tooth wave sample (t is 0..1), return value between -1 and 1
    static float32 Sawtooth(float32 t);
    /// get a square wave sample (t is 0..1), return value between -1 and 1
    static float32 Square(float32 t, float32 pulseWidth);
    /// get a noise wave sample (t is 0..1), return value between -1 and 1
    static float32 Noise(float32 t);
};

//------------------------------------------------------------------------------
inline int32
synth::TimeToTicks(float32 seconds, int32 freq) {
    return int32(seconds * float32(freq));
}

//------------------------------------------------------------------------------
inline float32
synth::TicksToTime(int32 ticks, int32 freq) {
    return float32(ticks) / float32(freq);
}

//------------------------------------------------------------------------------
inline float32
synth::SampleDuration(int32 freq) {
    return 1.0f / float32(freq);
}

//------------------------------------------------------------------------------
inline float32
synth::Sine(float32 t) {
    return std::sinf(t * M_PI_2);
}

//------------------------------------------------------------------------------
inline float32
synth::Square(float32 t, float32 pulseWidth) {
    return t <= pulseWidth ? +1.0f : -1.0f;
}

//------------------------------------------------------------------------------
inline float32
synth::Triangle(float32 t) {
    if (t < 0.5f) {
        // t: 0.0 -> 0.5,  ascending
        return (t - 0.25f) * 4.0f;
    }
    else {
        // t: 0.5 -> 1.0, descending
        return -((t - 0.75f) * 4.0f);
    }
}

//------------------------------------------------------------------------------
inline float32
synth::Sawtooth(float32 t) {
    return 1.0f - (t * 2.0f);
}

//------------------------------------------------------------------------------
inline float32
synth::Noise(float32 t) {
    // FIXME: this is not correct, it use perlin noise with
    // t as position instead!
    return float32((std::rand() & 0xFFFF) - 0x7FFF) / float32(0x8000);
}

} // namespace Synth
} // namespace Oryol
