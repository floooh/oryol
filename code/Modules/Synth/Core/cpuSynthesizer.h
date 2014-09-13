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
#include "Synth/Core/opBundle.h"

namespace Oryol {
namespace _priv {
    
class cpuSynthesizer {
public:
    /// synthesize!
    void Synthesize(const opBundle& bundle) const;

private:
    /// synthesize a single voice
    void synthesizeVoice(int32 voiceIndex, const opBundle& bundle) const;
    /// generate a single voice-track sample
    float32 sample(int32 curTick, const SynthOp* op) const;
};
    
} // namespace _priv
} // namespace Oryol