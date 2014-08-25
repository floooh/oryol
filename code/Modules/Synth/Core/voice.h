#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::voice
    @brief synthesize samples for a single voice made of multiple op-tracks
*/
#include "Core/Types.h"
#include "Synth/Core/SynthSetup.h"
#include "Synth/Core/Op.h"
#include "Synth/Core/synth.h"
#include "Synth/Core/voiceTrack.h"
#include "Synth/Core/opBundle.h"

namespace Oryol {
namespace Synth {
    
class voice {
public:
    /// constructor
    voice();
    /// destructor
    ~voice();
    
    /// setup the synthesizer object
    void Setup(int32 voiceIndex, const SynthSetup& setupAttrs);
    /// discard the synthesizer object
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;
    
    /// add new op to end of track
    void AddOp(int32 track, const Op& op);
    /// gather ops in tick range into opBundle
    void GatherOps(int32 startTick, int32 endTick, opBundle& inOutBundle);

private:
    bool isValid;
    int32 voiceIndex;
    voiceTrack tracks[synth::NumTracks];
};

//------------------------------------------------------------------------------
inline bool
voice::IsValid() const {
    return this->isValid;
}

} // namespace Synth
} // namespace Oryol
