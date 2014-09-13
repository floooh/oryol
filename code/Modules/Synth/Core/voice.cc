//------------------------------------------------------------------------------
//  voice.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "voice.h"
#include "Core/Assert.h"
#include "Synth/Core/synth.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
voice::voice() :
isValid(false),
voiceIndex(InvalidIndex) {
    // empty
}

//------------------------------------------------------------------------------
voice::~voice() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
voice::Setup(int32 vcIndex, const SynthSetup& setupAttrs) {
    o_assert_dbg(!this->isValid);
    o_assert_range_dbg(vcIndex, synth::NumVoices);
    
    this->voiceIndex = vcIndex;
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
voice::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    for (auto& track : this->tracks) {
        track.DiscardAllOps();
    }
}

//------------------------------------------------------------------------------
void
voice::AddOp(int32 track, const SynthOp& op) {
    o_assert_dbg(this->isValid);
    o_assert_range_dbg(track, synth::NumTracks);
    this->tracks[track].AddOp(op);
}

//------------------------------------------------------------------------------
void
voice::GatherOps(int32 startTick, int32 endTick, opBundle& inOutBundle) {
    o_assert_dbg(this->isValid);
    for (int trackIndex = 0; trackIndex < synth::NumTracks; trackIndex++) {
        this->tracks[trackIndex].GatherOps(startTick, endTick,
            inOutBundle.Begin[this->voiceIndex][trackIndex],
            inOutBundle.End[this->voiceIndex][trackIndex]);
    }
}

} // namespace _priv
} // namespace Oryol
