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

    this->effectFactory.setup(setup);
    this->effectPool.Setup(0, setup.SoundEffectPoolSize);
    resourceContainerBase::setup(setup.ResourceLabelStackCapacity, setup.ResourceRegistryCapacity);
}

//------------------------------------------------------------------------------
void
soundResourceContainer::discard() {
    o_assert_dbg(this->isValid());

    resourceContainerBase::discard();
    this->effectPool.Discard();
    this->effectFactory.discard();
}

//------------------------------------------------------------------------------
Id
soundResourceContainer::Create(const SoundEffectSetup& setup) {
    o_assert_dbg(this->isValid());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->effectPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        soundEffect& res = this->effectPool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->effectFactory.setupResource(res);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->effectPool.UpdateState(resId, newState);
    }
    return resId;
}

//------------------------------------------------------------------------------
Id
soundResourceContainer::Create(const SoundEffectSetup& setup, const void* data, int size) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(nullptr != data);
    o_assert_dbg(size > 0);

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->effectPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        soundEffect& res = this->effectPool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->effectFactory.setupResource(res, data, size);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->effectPool.UpdateState(resId, newState);
    }
    return resId;
}

//------------------------------------------------------------------------------
void
soundResourceContainer::Destroy(ResourceLabel label) {
    o_assert_dbg(this->isValid());

    Array<Id> ids = this->registry.Remove(label);
    for (const Id& id : ids) {
        if (ResourceState::Valid == this->effectPool.QueryState(id)) {
            soundEffect* effect = this->effectPool.Lookup(id);
            if (effect) {
                this->effectFactory.destroyResource(*effect);
            }
        }
        this->effectPool.Unassign(id);
    }
}

//------------------------------------------------------------------------------
soundEffect*
soundResourceContainer::lookupSoundEffect(const Id& resId) {
    o_assert_dbg(this->isValid());
    return this->effectPool.Lookup(resId);
}

} // namespace _priv
} // namespace Oryol
