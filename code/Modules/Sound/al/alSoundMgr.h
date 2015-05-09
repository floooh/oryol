#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::alSoundMgr
    @ingroup _priv
    @brief OpenAL implementation of soundMgr
*/
#include "Sound/Core/soundMgrBase.h"
#include "Sound/al/sound_al.h"

namespace Oryol {
namespace _priv {

class alSoundMgr : public soundMgrBase {
public:
    /// constructor
    alSoundMgr();
    /// destructor
    ~alSoundMgr();

    /// setup the sound manager
    void setup(const SoundSetup& setup, soundEffectPool* sndEffectPool);
    /// discard the sound manager
    void discard();

private:
    /// print info about OpenAL implementation
    void printALInfo();

    ALCdevice* alcDevice;
    ALCcontext* alcContext;    
};

} // namespace _priv
} // Oryol