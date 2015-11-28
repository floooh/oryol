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

    /// update the main volume (0.0f .. 1.0f)
    void UpdateVolume(float32 vol);
    /// update the sound system
    void Update();
    
    /// add an op to a voice track
    void AddOp(int32 voice, int32 track, const SynthOp& op, int32 timeOffset);
    
    bool isValid;
    SynthSetup setup;
    int32 curTick;
    voice voices[synth::NumVoices];
    cpuSynthesizer cpuSynth;
};
    
} // namespace _priv
} // namespace Oryol