#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::SynthFacade
    @brief the Synth module implements a chip-tune audio system
*/
#include "Core/Singleton.h"
#include "Synth/Core/soundMgr.h"
#include "Synth/Core/Sound.h"

namespace Oryol {
namespace Synth {
    
class SynthFacade {
    OryolLocalSingletonDecl(SynthFacade);
public:
    /// constructor
    SynthFacade(const SynthSetup& setupAttrs);
    /// destructor
    ~SynthFacade();

    /// update the sound system, call once per frame, advances tick
    void Update();
    /// get the current audio tick
    int32 CurrentTick() const;

    /// push a sound effect onto a voice's queue
    void Play(uint32 voice, const Sound& sound, float32 freq, float32 vol, float32 timeOffset);
    /// stop a voices
    void Stop(uint32 voice, int32 tickOffset=0);
    /// pause a voice
    void Pause(uint32 voice, int32 tickOffset=0);
    /// continue a voice
    void Continue(uint32 voiceMask, int32 tickOffset=0);
    /// volume fade for one or more voices
    void VolumeFade(uint32 voiceMask, float32 targetVolume);
    
private:
    int32 curTick;
    soundMgr soundManager;
};
    
} // namespace Sound
} // namespace Oryol