#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::soundResourceContainer
    @ingroup _priv
    @brief resource container for Sound module
*/
#include "Resource/Core/resourceContainerBase.h"
#include "Sound/Core/SoundSetup.h"
#include "Sound/Core/SoundEffectSetup.h"
#include "Sound/Core/soundEffectPool.h"
#include "Sound/Core/soundEffectFactory.h"

namespace Oryol {
namespace _priv {

class soundResourceContainer : public resourceContainerBase {
public:
    /// setup the resource container
    void setup(const SoundSetup& setup);
    /// discard the resource container
    void discard();

    /// create sound effect resource
    Id Create(const SoundEffectSetup& setup);
    /// create sound effect resource with raw data
    Id Create(const SoundEffectSetup& setup, const void* data, int size);
    /// destroy resources by label
    void Destroy(ResourceLabel label);

    /// lookup soundEffect, return 0 if not exists or valid
    soundEffect* lookupSoundEffect(const Id& resId);

    _priv::soundEffectPool effectPool;
    _priv::soundEffectFactory effectFactory;
};

} // namespace _pric
} // namespace Oryol
