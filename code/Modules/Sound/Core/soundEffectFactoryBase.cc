//------------------------------------------------------------------------------
//  soundEffectFactoryBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "soundEffectFactoryBase.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
soundEffectFactoryBase::soundEffectFactoryBase() :
valid(false) {
    // empty
}

//------------------------------------------------------------------------------
soundEffectFactoryBase::~soundEffectFactoryBase() {
    o_assert_dbg(!this->isValid());
}

//------------------------------------------------------------------------------
void
soundEffectFactoryBase::setup(const SoundSetup& /*setup*/) {
    o_assert_dbg(!this->isValid());
    this->valid = true;
}

//------------------------------------------------------------------------------
void
soundEffectFactoryBase::discard() {
    o_assert_dbg(this->isValid());
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
soundEffectFactoryBase::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
soundEffectFactoryBase::setupResource(soundEffect& /*effect*/) {
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
soundEffectFactoryBase::setupResource(soundEffect& /*effect*/, const void* /*data*/, int /*size*/) {
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
soundEffectFactoryBase::destroyResource(soundEffect& effect) {
    effect.Clear();
}

} // namespace _priv
} // namespace Oryol