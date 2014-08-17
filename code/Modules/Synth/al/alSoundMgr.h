#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::alSoundMgr
    @brief OpenAL sound system wrapper
*/
#include "Synth/base/soundMgrBase.h"
#include "OpenAL/al.h"
#include "OpenAL/alc.h"

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

private:
    ALCdevice* alcDevice;
    ALCcontext* alcContext;
};
    
} // namespace Synth
} // namespace Oryol