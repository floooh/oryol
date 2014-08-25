#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::soundMgrBase
    @brief sound manager base class
*/
#include "Synth/Core/SynthSetup.h"
#include "Synth/Core/Op.h"
#include "Synth/Core/voice.h"
#include "Synth/Core/cpuSynthesizer.h"
#include "Synth/Core/gpuSynthesizer.h"

namespace Oryol {
namespace Synth {
    
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
    void AddOp(int32 voice, int32 track, const Op& op, float32 timeOffset);
    
protected:
    bool isValid;
    SynthSetup setup;
    int32 curTick;
    voice voices[synth::NumVoices];
    cpuSynthesizer cpuSynth;
    gpuSynthesizer gpuSynth;
};
    
} // namespace Synth
} // namespace Oryol