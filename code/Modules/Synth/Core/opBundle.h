#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::opBundle
    @brief holds all info needed to fill a sample buffer
*/
#include "Synth/Core/Op.h"
#include "Synth/Core/synth.h"

namespace Oryol {
namespace Synth {
    
class opBundle {
public:
    /// sample buffer start tick
    int32 StartTick[synth::NumVoices] = { 0 };
    /// sample buffer end tick
    int32 EndTick[synth::NumVoices] = { 0 };
    /// pointers to start op
    Op* Begin[synth::NumVoices][synth::NumTracks] = { { nullptr } };
    /// one-past-end-pointers to end op
    Op* End[synth::NumVoices][synth::NumTracks] = { { nullptr } };
    /// sample buffer pointers
    void* Buffer[synth::NumVoices] = { nullptr };
    /// sample buffer size in bytes
    int32 BufferNumBytes = 0;
};
    
} // namespace Synth
} // namespace Oryol