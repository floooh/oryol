//------------------------------------------------------------------------------
//  soundResourceContainer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "soundResourceContainer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
soundResourceContainer::setup(const SoundSetup& setup) {
    o_assert_dbg(!this->isValid());

    this->soundEffectPool.Setup(0, setup.SoundEffectPoolSize);
    resourceContainerBase::setup(setup.ResourceLabelStackCapacity, setup.ResourceRegistryCapacity);
}

//------------------------------------------------------------------------------
void
soundResourceContainer::discard() {
    o_assert_dbg(this->isValid());

    resourceContainerBase::discard();
    this->soundEffectPool.Discard();
}

} // namespace _priv
} // namespace Oryol
