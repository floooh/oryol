#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::SynthSetup
    @ingroup Synth
    @brief setup parameters for the Synth subsystem
    
    NOTE: the SampleRate vs BufferNumSamples values must be carefully tuned
    to avoid audio glitches and too high latencies. In general the buffer-
    size should be big enough to play at least 2 frames of audio at the 
    provided sample-rate.
*/
#include "Core/Types.h"

namespace Oryol {
    
class SynthSetup {
public:
    /// initial volume
    float32 InitialVolume = 0.05f;
    /// playback sample rate
    int32 SampleRate = 44100;
    /// number of samples in one streaming buffer (must be at least SampleRate / fps)
    int32 NumBufferSamples = (SampleRate / 30);
};
    
} // namespace Oryol