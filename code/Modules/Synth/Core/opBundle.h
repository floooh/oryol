#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::opBundle
    @ingroup _priv
    @brief holds all info needed to fill a sample buffer
*/
#include "Synth/Core/SynthOp.h"
#include "Synth/Core/synth.h"

namespace Oryol {
namespace _priv {
    
class opBundle {
public:
    /// constructor
    opBundle() :
        BufferNumBytes(0) {
        
        // this is a workaround for VS2013 missing array initializers :/
        for (int voice = 0; voice < synth::NumVoices; voice++) {
            this->StartTick[voice] = 0;
            this->EndTick[voice] = 0;
            this->Buffer[voice] = nullptr;
            for (int track = 0; track < synth::NumTracks; track++) {
                this->Begin[voice][track] = nullptr;
                this->End[voice][track] = nullptr;
            }
        }
    };

    /// sample buffer start tick
    int32 StartTick[synth::NumVoices];
    /// sample buffer end tick
    int32 EndTick[synth::NumVoices];
    /// pointers to start op
    SynthOp* Begin[synth::NumVoices][synth::NumTracks];
    /// one-past-end-pointers to end op
    SynthOp* End[synth::NumVoices][synth::NumTracks];
    /// sample buffer pointers
    void* Buffer[synth::NumVoices];
    /// sample buffer size in bytes
    int32 BufferNumBytes;
};
    
} // namespace _priv
} // namespace Oryol