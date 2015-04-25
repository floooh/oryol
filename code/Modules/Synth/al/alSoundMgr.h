#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::alSoundMgr
    @ingroup _priv
    @brief OpenAL sound system wrapper
*/
#include "Synth/base/soundMgrBase.h"
#include "Synth/al/al.h"
#include "Synth/al/alBufferStreamer.h"

namespace Oryol {
namespace _priv {
    
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
    /// update the main volume (0.0f .. 1.0f)
    void UpdateVolume(float32 vol);
    /// update the sound system
    void Update();
    
private:
    /// print AL implementation info
    void PrintALInfo();
    
    ALCdevice* alcDevice;
    ALCcontext* alcContext;
    alBufferStreamer streamer;
};
    
} // namespace _priv
} // namespace Oryol