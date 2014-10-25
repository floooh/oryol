#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::synth
    @ingroup _priv
    @brief audio synthesizing utility functions
    
    FIXME: most of this stuff should eventually go onto the GPU,
    FIXME: before that, at least move the waveform functions into 
    precomputed lookup tables
*/
#include "Core/Types.h"

namespace Oryol {
namespace _priv {
    
class synth {
public:
    /// standard sample rate
    static const int32 SampleRate = 44100;
    /// byte size of a single sample
    static const int32 SampleSize = 2;
    /// number of samples in one streaming buffer
    static const int32 BufferNumSamples = 2 * 1024;
    /// byte size of one streaming buffer
    static const int32 BufferSize = SampleSize * BufferNumSamples;
    /// number of voices
    static const int32 NumVoices = 1;
    /// number of tracks per voice
    static const int32 NumTracks = 4;
    /// max sample value (16 bit signed)
    static const int32 MaxSampleVal = (1<<15) - 1;
    /// min sample value (16 bit signed)
    static const int32 MinSampleVal = -((1<<15) - 1);
    
    /// convert time in seconds to sample ticks
    static int32 TimeToTicks(float32 seconds, int32 freq);
    /// convert sample ticks to time in seconds
    static float32 TicksToTime(int32 ticks, int32 freq);
    /// get sample duration in seconds
    static float32 SampleDuration(int32 freq);
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

} // namespace _priv
} // namespace Oryol
