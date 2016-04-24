#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::alSoundEffectFactory
    @ingroup _priv
    @brief OpenAL implementation of sound effect factory
*/
#include "Sound/Core/soundEffectFactoryBase.h"

namespace Oryol {
namespace _priv {

class alSoundEffectFactory : public soundEffectFactoryBase {
public:
    /// setup resource
    ResourceState::Code setupResource(soundEffect& effect);
    /// setup with 'raw' data
    ResourceState::Code setupResource(soundEffect& effect, const void* data, int size);
    /// destroy a resource
    void destroyResource(soundEffect& effect);

private:
    /// create the alBuffer and alSources for soundEffect
    void createBufferAndSources(soundEffect& effect, const int16_t* samples, int numSamples);

    static const int MaxNumBufferSamples = 512 * 1024;
    int16_t sampleBuffer[MaxNumBufferSamples];
};

} // namespace _priv
} // namespace Oryol