//------------------------------------------------------------------------------
//  soundEffectFactoryBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "soundEffectFactoryBase.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
ResourceState::Code
soundEffectFactoryBase::setupResource(soundEffect& /*effect*/) {
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
soundEffectFactoryBase::setupResource(soundEffect& /*effect*/, const void* /*data*/, int32 /*size*/) {
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
soundEffectFactoryBase::destroyResource(soundEffect& effect) {
    effect.Clear();
}

} // namespace _priv
} // namespace Oryol