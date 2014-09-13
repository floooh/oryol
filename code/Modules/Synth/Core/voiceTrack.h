#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::voiceTrack
    @brief a single track of sound synthesize ops
    
    A voiceTrack is a simple sequence of sound Ops (basically an 
    sound synthesis instruction stream). Ops overlap for a cross-fade
    period, when the time cursor is over a cross-fade section, 2
    Ops must be evaluated and will be mixed.
*/
#include "Synth/Core/Op.h"
#include "Core/Containers/Array.h"

namespace Oryol {
namespace Synth {

class voiceTrack {
public:
    /// constructor
    voiceTrack();
    /// destructor
    ~voiceTrack();
    
    /// append a new op to the track (sorted by start tick of op)
    void AddOp(const Op& op);
    /// discard all ops
    void DiscardAllOps();
    /// gather ops between which overlap a tick range, outOpBegin can be nullptr, outOpEnd is 'one-past-ptr'!
    void GatherOps(int32 startTick, int32 endTick, Op*& outOpBegin, Op*& outOpEnd);
    
private:
    /// discard ops behind the play cursor (these no longer influence sound output)
    void discardExpiredOps(int32 tick);

    Array<Op> track;
};

} // namespace Synth
} // namespace Oryol