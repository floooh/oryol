#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::SynthFacade
    @ingroup Synth
    @brief the Synth module implements a chip-tune audio system
*/
#include "Core/Singleton.h"
#include "Synth/Core/soundMgr.h"
#include "Synth/Core/SynthOp.h"

namespace Oryol {
    
class SynthFacade {
    OryolLocalSingletonDecl(SynthFacade);
public:
    /// constructor
    SynthFacade(const SynthSetup& setupAttrs);
    /// destructor
    ~SynthFacade();

    /// update the sound system, call once per frame, advances tick
    void Update();

    /// add a sound synthesis Op
    void AddOp(int32 voice, int32 track, const SynthOp& op, float32 timeOffset = 0.0f);
    
private:
    _priv::soundMgr soundManager;
};
    
} // namespace Oryol