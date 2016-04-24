#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::soundMgrBase
    @ingroup _priv
    @brief sound manager base class
*/
#include "Sound/Core/SoundSetup.h"

namespace Oryol {
namespace _priv {

class soundEffectPool;
class soundEffect;

class soundMgrBase {
public:
    /// constructor
    soundMgrBase();
    /// destructor
    ~soundMgrBase();

    /// setup the sound manager
    void setup(const SoundSetup& setup, soundEffectPool* sndEffectPool);
    /// discard the sound manager
    void discard();
    /// return true if sound manager has been setup
    bool isValid() const;

    /// play a sound effect
    int play(soundEffect* effect, int loopCount, float pitch, float volume);
    /// stop a sound effect voice
    void stop(soundEffect* effect, int voice);
    /// set pitch-shift on playing sound
    void setPitch(soundEffect* effect, int voice, float pitch);
    /// set volume on playing sound
    void setVolume(soundEffect* effect, int voice, float volume);

protected:
    bool valid;
    soundEffectPool* effectPool;
};

} // namespace _priv
} // namespace Oryol