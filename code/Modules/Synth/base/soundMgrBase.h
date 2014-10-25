#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::soundMgrBase
    @ingroup _priv
    @brief sound manager base class
*/
#include "Synth/Core/SynthSetup.h"
#include "Synth/Core/SynthOp.h"
#include "Synth/Core/voice.h"
#include "Synth/Core/cpuSynthesizer.h"
#include "Synth/Core/gpuSynthesizer.h"

namespace Oryol {
namespace _priv {
    
class soundMgrBase {
public:
    /// constructor
    soundMgrBase();
    /// destructor
    ~soundMgrBase();
    
    /// setup the sound system
    void Setup(const SynthSetup& setupAttrs);
    /// discard the sound system
    void Discard();
    /// return true if has been setup
    bool IsValid() const;
    
    /// update the sound system
    void Update();
    
    /// add an op to a voice track
    void AddOp(int32 voice, int32 track, const SynthOp& op, int32 timeOffset);
    
protected:
    bool isValid;
    SynthSetup setup;
    bool useGpuSynth;
    int32 curTick;
    voice voices[synth::NumVoices];
    cpuSynthesizer cpuSynth;
    gpuSynthesizer gpuSynth;
};
    
} // namespace _priv
} // namespace Oryol