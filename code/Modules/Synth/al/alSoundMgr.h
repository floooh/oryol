#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::alSoundMgr
    @brief OpenAL sound system wrapper
*/
#include "Synth/base/soundMgrBase.h"
#include "Synth/al/al.h"
#include "Synth/al/alBufferStreamer.h"

namespace Oryol {
namespace Synth {
    
class alSoundMgr : public soundMgrBase {
public:
    /// constructor
    alSoundMgr();
    /// destructor
    ~alSoundMgr();
    
    /// setup the sound system
    void Setup(const SynthSetup& setupAttrs);
    /// discard the sound system
    void Discard();
    /// update the sound system
    void Update();
    
private:
    /// print AL implementation info
    void PrintALInfo();
    
    ALCdevice* alcDevice;
    ALCcontext* alcContext;
    alBufferStreamer streamer;
};
    
} // namespace Synth
} // namespace Oryol