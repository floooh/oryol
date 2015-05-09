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
    // FIXME!
    return ResourceState::Failed;
}

//------------------------------------------------------------------------------
ResourceState::Code
alSoundEffectFactory::setupResource(soundEffect& effect, const void* data, int32 size) {
    // FIXME!
    return ResourceState::Failed;
}

//------------------------------------------------------------------------------
void
alSoundEffectFactory::destroyResource(soundEffect& effect) {
    // FIXME!
    effect.Clear();
}

} // namespace _priv
} // namespace Oryol