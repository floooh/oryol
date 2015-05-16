//------------------------------------------------------------------------------
//  alSoundMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "alSoundMgr.h"
#include "Sound/al/sound_al.h"
#include "Core/Assertion.h"
#include "Core/String/StringBuilder.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
alSoundMgr::alSoundMgr() :
alcDevice(nullptr),
alcContext(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
alSoundMgr::~alSoundMgr() {
    o_assert_dbg(nullptr == this->alcDevice);
    o_assert_dbg(nullptr == this->alcContext);
}

//------------------------------------------------------------------------------
void
alSoundMgr::setup(const SoundSetup& setup, soundEffectPool* sndEffectPool) {
    o_assert_dbg(!this->isValid());
    o_assert_dbg(nullptr == this->alcDevice);
    o_assert_dbg(nullptr == this->alcContext);

    soundMgrBase::setup(setup, sndEffectPool);

    // setup OpenAL context and make it current
    this->alcDevice = alcOpenDevice(NULL);
    if (nullptr == this->alcDevice) {
        o_warn("alcOpenDevice() failed!\n");
        return;
    }
    this->alcContext = alcCreateContext(this->alcDevice, NULL);
    if (nullptr == this->alcContext) {
        o_warn("alcCreateContext() failed!\n");
        return;
    }
    if (!alcMakeContextCurrent(this->alcContext)) {
        o_warn("alcMakeContextCurrent() failed!\n");
        return;
    }
    this->printALInfo();

    this->valid = true;
}

//------------------------------------------------------------------------------
void
alSoundMgr::discard() {
    o_assert_dbg(this->isValid());

    if (nullptr != this->alcContext) {
        alcDestroyContext(this->alcContext);
        this->alcContext = nullptr;
    }
    if (nullptr != this->alcDevice) {
        alcCloseDevice(this->alcDevice);
        this->alcDevice = nullptr;
    }
    soundMgrBase::discard();
}

//------------------------------------------------------------------------------
void
alSoundMgr::printALInfo() {
    const ALCchar* alcStr = alcGetString(this->alcDevice, ALC_DEVICE_SPECIFIER);
    if (alcStr) {
        Log::Info("ALC_DEVICE_SPECIFIER: %s\n", alcStr);
    }
    alcStr = alcGetString(this->alcDevice, ALC_EXTENSIONS);
    if (alcStr) {
        StringBuilder strBuilder(alcStr);
        strBuilder.SubstituteAll(" ", "\n");
        Log::Info("ALC_EXTENSIONS:\n%s\n", strBuilder.AsCStr());
    }
    const ALchar* alStr = alGetString(AL_VERSION);
    if (alStr) {
        Log::Info("AL_VERSION: %s\n", alStr);
    }
    alStr = alGetString(AL_RENDERER);
    if (alStr) {
        Log::Info("AL_RENDERER: %s\n", alStr);
    }
    alStr = alGetString(AL_VENDOR);
    if (alStr) {
        Log::Info("AL_VENDOR: %s\n", alStr);
    }
    alStr = alGetString(AL_EXTENSIONS);
    if (alStr) {
        StringBuilder strBuilder(alStr);
        strBuilder.SubstituteAll(" ", "\n");
        Log::Info("AL_EXTENSIONS:\n%s\n", strBuilder.AsCStr());
    }
}

//------------------------------------------------------------------------------
void
alSoundMgr::play(soundEffect* effect, int32 loopCount, int32 freqShift) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(nullptr != effect);
    o_assert_dbg(effect->State == ResourceState::Valid);
    o_assert_dbg(effect->nextSourceIndex < effect->numSources);

    // get next source, round-robin
    ALuint src = effect->alSources[effect->nextSourceIndex++];
    if (effect->nextSourceIndex == effect->numSources) {
        effect->nextSourceIndex = 0;
    }
    o_assert_dbg(0 != src);
    alSourcePlay(src);
    ORYOL_SOUND_AL_CHECK_ERROR();
}

} // namespace _priv
} // namespace Oryol