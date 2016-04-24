#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::alSoundMgr
    @ingroup _priv
    @brief OpenAL implementation of soundMgr
*/
#include "Sound/Core/soundMgrBase.h"
#include "Sound/Core/soundEffect.h"
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

    /// play a sound effect
    int play(soundEffect* effect, int loopCount, float pitch, float volume);
    /// stop a sound effect voice
    void stop(soundEffect* effect, int voice);
    /// set pitch-shift on playing sound
    void setPitch(soundEffect* effect, int voice, float pitch);
    /// set volume on playing sound
    void setVolume(soundEffect* effect, int voice, float volume);

private:
    /// print info about OpenAL implementation
    void printALInfo();

    ALCdevice* alcDevice;
    ALCcontext* alcContext;    
};

} // namespace _priv
} // Oryol