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
};
    
} // namespace Synth
} // namespace Oryol