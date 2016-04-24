#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::soundEffectFactoryBase
    @ingroup _priv
    @brief base class for sound effect factories
*/
#include "Sound/Core/soundEffect.h"
#include "Sound/Core/SoundSetup.h"
#include "Sound/Core/SoundEffectSetup.h"

namespace Oryol {
namespace _priv {

class soundEffectFactoryBase {
public:
    /// constructor
    soundEffectFactoryBase();
    /// destructor
    ~soundEffectFactoryBase();

    /// setup the factory
    void setup(const SoundSetup& setup);
    /// discard the factory
    void discard();
    /// return true if factory had been setup
    bool isValid() const;

    /// setup resource
    ResourceState::Code setupResource(soundEffect& effect);
    /// setup with 'raw' data
    ResourceState::Code setupResource(soundEffect& effect, const void* data, int size);
    /// destroy a resource
    void destroyResource(soundEffect& effect);

protected:
    bool valid;
};

} // namespace _priv
} // namespace Oryol
