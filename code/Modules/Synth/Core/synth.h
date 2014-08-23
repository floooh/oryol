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

} // namespace Synth
} // namespace Oryol
