#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup Sound Sound
    @brief Sound Playback Module

    @class Oryol::Sound
    @ingroup Sound
    @brief audio module for generated sound effects and short samples
*/
#include "Core/Types.h"
#include "Core/Containers/Buffer.h"
#include "Sound/Core/soundResourceContainer.h"
#include "Sound/Core/SoundSetup.h"
#include "Sound/Core/soundMgr.h"
#include "Sound/Core/soundResourceContainer.h"
#include "Resource/Core/SetupAndData.h"

namespace Oryol {

class Sound {
public:
    /// voice handle, returned by Play()
    typedef int Voice;
    /// invalid voice handle
    static const int InvalidVoice = -1;

    /// setup the Sound module
    static void Setup(const SoundSetup& setup);
    /// discard the Sound module
    static void Discard();
    /// check if Sound module is valid
    static bool IsValid();

    /// generate new resource label and push on label stack
    static ResourceLabel PushResourceLabel();
    /// push explicit resource label on label stack
    static void PushResourceLabel(ResourceLabel label);
    /// pop resource label from label stack
    static ResourceLabel PopResourceLabel();
    /// create a sound effect resource
    static Id CreateResource(const SoundEffectSetup& setup);
    /// create a resource object with data in stream object
    static Id CreateResource(const SetupAndData<SoundEffectSetup>& setupAndData);
    /// create a sound effect resource with data in stream
    static Id CreateResource(const SoundEffectSetup& setup, const Buffer& data);
    /// create a sound effect resource with raw data
    static Id CreateResource(const SoundEffectSetup& setup, const void* data, int size);
    /// lookup a resource id by Locator
    static Id LookupResource(const Locator& locator);
    /// destroy one or several sound resources by matching label
    static void DestroyResources(ResourceLabel label);

    /// play a sound effect, return Voice handle
    static Voice Play(Id snd, int loopCount=1, float pitch=1.0f, float volume=1.0f);
    /// stop playback of sound effect voice
    static void Stop(Id snd, Voice voice);
    /// set the frequency-shift of a playing sound
    static void SetPitch(Id snd, Voice voice, float pitch);
    /// set the volume of a playing sound
    static void SetVolume(Id snd, Voice voice, float volume);

private:
    struct _state {
        SoundSetup soundSetup;
        _priv::soundMgr soundMgr;
        _priv::soundResourceContainer resourceContainer;
    };
    static _state* state;
};

} // namespace Oryol
