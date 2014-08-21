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

    /// push a sound effect onto a voice's queue
    void Play(uint32 voice, const Sound& sound, float32 timeOffset = 0.0f);
    
private:
    soundMgr soundManager;
};
    
} // namespace Sound
} // namespace Oryol