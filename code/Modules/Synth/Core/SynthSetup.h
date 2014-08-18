#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::SynthSetup
    @brief setup parameters for the Synth subsystem
*/
#include "Core/Types.h"

namespace Oryol {
namespace Synth {
    
class SynthSetup {
public:
    /// number of voices
    int32 NumVoices = 4;
    /// number of samples in waveforms (default: 32)
    int32 WaveFormNumSamples = 32;
    /// output sample rate
    int32 SampleRate = 44100;
    /// max output latency in seconds
    float32 Latency = 1.0f / 15.0f;
};
    
} // namespace Synth
} // namespace Oryol