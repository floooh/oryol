#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup Synth Synth
    @brief chip-tune audio synthesis

    @class Oryol::Synth
    @ingroup Synth
    @brief Synth module facade
*/
#include "Synth/Core/soundMgr.h"
#include "Synth/Core/SynthOp.h"

namespace Oryol {
    
class Synth {
public:
    /// setup the Synth module
    static void Setup(const SynthSetup& setup);
    /// discard the Synth module
    static void Discard();
    /// check if Synth module is valid
    static bool IsValid();
    /// access to the SynthSetup object
    static const class SynthSetup& SynthSetup();
    /// update the global volume (0.0f .. 1.0f)
    static void UpdateVolume(float32 vol);
    /// update the sound system, call once per frame, advances tick
    static void Update();
    /// add a sound synthesis Op
    static void AddOp(int32 voice, int32 track, const SynthOp& op, int32 timeOffset = 0);
    
private:
    struct _state {
        _priv::soundMgr soundManager;
    };
    static _state* state;
};
    
} // namespace Oryol