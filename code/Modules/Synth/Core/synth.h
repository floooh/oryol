#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::synth
    @ingroup _priv
    @brief audio synthesizing utility functions
    
    NOTE: the sample rate and buffer size must remain carefully tuned in 
    relation to each other for max latency versus giving the sound API 
    enough time to queue the buffers.
*/
#include "Core/Types.h"

namespace Oryol {
namespace _priv {
    
class synth {
public:
    /// number of voices (configured via cmake option)
    static const int32 NumVoices = ORYOL_SYNTH_NUM_VOICES;
    /// number tracks per voice (configured via cmake option)
    static const int32 NumTracks = ORYOL_SYNTH_NUM_TRACKS_PER_VOICE;
    /// max sample value (16 bit signed)
    static const int32 MaxSampleVal = (1<<15) - 1;
    /// min sample value (16 bit signed)
    static const int32 MinSampleVal = -((1<<15) - 1);
};

} // namespace _priv
} // namespace Oryol
