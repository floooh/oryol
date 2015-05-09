#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::soundEffectFactoryBase
    @ingroup _priv
    @brief base class for sound effect factories
*/
#include "Sound/Core/soundEffect.h"

namespace Oryol {
namespace _priv {

class soundEffectFactoryBase {
public:
    /// setup resource
    ResourceState::Code setupResource(soundEffect& effect);
    /// setup with 'raw' data
    ResourceState::Code setupResource(soundEffect& effect, const void* data, int32 size);
    /// destroy a resource
    void destroyResource(soundEffect& effect);
};

} // namespace _priv
} // namespace Oryol
