//------------------------------------------------------------------------------
//  alSoundEffectFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "alSoundEffectFactory.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
ResourceState::Code
alSoundEffectFactory::setupResource(soundEffect& effect) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(effect.Setup.NumVoices <= SoundEffectSetup::MaxNumVoices);

    // compute number of samples
    const int numSamples = int(effect.Setup.Duration * effect.Setup.BufferFrequency);
    o_assert_dbg(numSamples > 0);
    o_assert2(numSamples <= MaxNumBufferSamples, "Too many samples in sound effect!\n");

    // call optional the sample-func to generate samples
    if (effect.Setup.SampleFunc) {
        const float dt = 1.0f / effect.Setup.BufferFrequency; // FIXME: will we run into precision problems with big buffers?
        effect.Setup.SampleFunc(dt, this->sampleBuffer, numSamples);
    }
    else {
        // if no sample-func is provided, just fill the buffer with silence
        Memory::Clear(this->sampleBuffer, numSamples * sizeof(int16_t));
    }

    // create the alBuffer and alSources
    effect.numSources = effect.Setup.NumVoices;
    this->createBufferAndSources(effect, this->sampleBuffer, numSamples);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
alSoundEffectFactory::setupResource(soundEffect& effect, const void* data, int size) {
    o_error("alSoundEffectFactory::setupResource() from data not yet implemented!\n");
    return ResourceState::Failed;
}

//------------------------------------------------------------------------------
void
alSoundEffectFactory::destroyResource(soundEffect& effect) {
    o_assert_dbg(this->isValid());

    ORYOL_SOUND_AL_CHECK_ERROR();
    if (0 != effect.alSources[0]) {
        alDeleteSources(effect.numSources, &(effect.alSources[0]));
        ORYOL_SOUND_AL_CHECK_ERROR();
    }
    if (0 != effect.alBuffer) {
        alDeleteBuffers(1, &effect.alBuffer);
        ORYOL_SOUND_AL_CHECK_ERROR();
    }
    effect.Clear();
}

//------------------------------------------------------------------------------
void
alSoundEffectFactory::createBufferAndSources(soundEffect& effect, const int16_t* samples, int numSamples) {
    o_assert_dbg(0 == effect.alBuffer);

    alGenBuffers(1, &effect.alBuffer);
    o_assert_dbg(0 != effect.alBuffer);
    ORYOL_SOUND_AL_CHECK_ERROR();
    alGenSources(effect.numSources, &(effect.alSources[0]));
    ORYOL_SOUND_AL_CHECK_ERROR();
    effect.nextSourceIndex = 0;
    if (numSamples > 0) {
        o_assert_dbg(nullptr != samples);
        alBufferData(effect.alBuffer, AL_FORMAT_MONO16, samples, numSamples * sizeof(int16_t), effect.Setup.BufferFrequency);
        ORYOL_SOUND_AL_CHECK_ERROR();
    }

    // attach buffer to sources
    for (int i = 0; i < effect.numSources; i++) {
        o_assert_dbg(0 != effect.alSources[i]);
        alSourcei(effect.alSources[i], AL_BUFFER, effect.alBuffer);
        ORYOL_SOUND_AL_CHECK_ERROR();
    }
}

} // namespace _priv
} // namespace Oryol