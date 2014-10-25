#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::cpuSynthesizer
    @ingroup _priv
    @brief the CPU-driven synthesizer core
    
    The cpuSynthesize class takes an opBundle object and fills sample
    buffers with samples (one for each voice). Samples are synthesized
    on the CPU.
*/
#include "Synth/Core/SynthSetup.h"
#include "Synth/Core/opBundle.h"

namespace Oryol {
namespace _priv {
    
class cpuSynthesizer {
public:
    /// setup the synthesizer
    void Setup(const SynthSetup& setupParams);
    /// synthesize!
    void Synthesize(const opBundle& bundle);

private:
    /// setup the wave samples
    void setupWaves();
    /// synthesize a single voice
    void synthesizeVoice(int32 voiceIndex, const opBundle& bundle);
    /// generate a single voice-track sample
    int32 sample(int32 voiceIndex, int32 trackIndex, const SynthOp* op);
    
    static const int32 NumWaveSamples = 32;
    int32 waves[SynthOp::NumCodes][NumWaveSamples];
    uint32 freqCounters[synth::NumVoices][synth::NumTracks];
};
    
} // namespace _priv
} // namespace Oryol