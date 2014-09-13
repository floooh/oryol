#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::SynthSetup
    @ingroup Synth
    @brief setup parameters for the Synth subsystem
*/
#include "Core/Types.h"

namespace Oryol {
    
class SynthSetup {
public:
    /// use GPU audio rendering
    bool UseGPUSynthesizer = false;
    /// number of voices
    int32 NumVoices = 4;
    /// number of samples in waveforms (default: 32)
    int32 WaveFormNumSamples = 32;
};
    
} // namespace Oryol