#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::voiceTrack
    @ingroup _priv
    @brief a single track of sound synthesize ops
    
    A voiceTrack is a simple sequence of sound Ops (basically an 
    sound synthesis instruction stream). Ops overlap for a cross-fade
    period, when the time cursor is over a cross-fade section, 2
    Ops must be evaluated and will be mixed.
*/
#include "Synth/Core/SynthOp.h"
#include "Core/Containers/Array.h"

namespace Oryol {
namespace _priv {

class voiceTrack {
public:
    /// constructor
    voiceTrack();
    /// destructor
    ~voiceTrack();
    
    /// append a new op to the track (sorted by start tick of op)
    void AddOp(const SynthOp& op);
    /// discard all ops
    void DiscardAllOps();
    /// gather ops between which overlap a tick range, outOpBegin can be nullptr, outOpEnd is 'one-past-ptr'!
    void GatherOps(int32 startTick, int32 endTick, SynthOp*& outOpBegin, SynthOp*& outOpEnd);
    
private:
    /// discard ops behind the play cursor (these no longer influence sound output)
    void discardExpiredOps(int32 tick);

    Array<SynthOp> track;
};

} // namespace _priv
} // namespace Oryol