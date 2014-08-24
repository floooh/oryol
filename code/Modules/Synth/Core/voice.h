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

namespace Oryol {
namespace Synth {
    
class voice {
public:
    /// constructor
    voice();
    /// destructor
    ~voice();
    
    /// setup the synthesizer object
    void Setup(const SynthSetup& setupAttrs);
    /// discard the synthesizer object
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;
    
    /// add new op to end of track
    void AddOp(int32 track, const Op& op);
    /// synthesize samples into memory buffer
    void Synthesize(int32 startTick, void* buffer, int32 bufNumBytes);

private:
    /// generate a single sample
    float32 sample(int32 curTick, const Op* op);

    bool isValid;
    voiceTrack tracks[synth::NumTracks];
    Op* trackOpBegin[synth::NumTracks];
    Op* trackOpEnd[synth::NumTracks];
};

//------------------------------------------------------------------------------
inline bool
voice::IsValid() const {
    return this->isValid;
}

} // namespace Synth
} // namespace Oryol
