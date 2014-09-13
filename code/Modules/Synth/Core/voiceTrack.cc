//------------------------------------------------------------------------------
//  voiceTrack.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "voiceTrack.h"
#include <algorithm>

namespace Oryol {
namespace Synth {
    
//------------------------------------------------------------------------------
voiceTrack::voiceTrack() {
    this->track.Reserve(32);
}

//------------------------------------------------------------------------------
voiceTrack::~voiceTrack() {
    // empty
}

//------------------------------------------------------------------------------
void
voiceTrack::AddOp(const Op& op) {

    // find insertion position, and truncate the track (discard any ops after it)
    if (!this->track.Empty()) {
        auto ptr = std::lower_bound(this->track.begin(), this->track.end(), op);
        int32 index = ptr - this->track.begin();
        while (index < this->track.Size()) {
            this->track.Erase(this->track.Size() - 1);
            Log::Warn("voiceTrack::Add(): track was truncated\n");
        }
    }
    
    // fix the end tick of the previous op
    if (!this->track.Empty()) {
        Op& prev = this->track.Back();
        prev.endTick = op.startTick + op.FadeInTicks;
    }
    this->track.Add(op);
}

//------------------------------------------------------------------------------
void
voiceTrack::discardExpiredOps(int32 tick) {
    while (!this->track.Empty() && (tick > track.Front().endTick)) {
        this->track.Erase(0);
    }
}

//------------------------------------------------------------------------------
void
voiceTrack::DiscardAllOps() {
    this->track.Clear();
}

//------------------------------------------------------------------------------
void
voiceTrack::GatherOps(int32 startTick, int32 endTick, Op*& outOpBegin, Op*& outOpEnd) {
    outOpBegin = nullptr;
    outOpEnd = nullptr;
    
    // throw away any expired ops
    this->discardExpiredOps(startTick);
    
    // gather all ops with startTick < endTick
    if (!this->track.Empty()) {
        outOpBegin = &this->track.Front();
        outOpEnd = outOpBegin;
        const int numOps = this->track.Size();
        for (int i = 0; i < numOps; i++) {
            if (this->track[i].startTick < endTick) {
                outOpEnd++;
            }
            else {
                break;
            }
        }
    }
}

} // namespace Synth
} // namespace Oryol