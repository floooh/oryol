//------------------------------------------------------------------------------
//  GfxResourceContainer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "GfxResourceContainer.h"
#include "Core/Core.h"

namespace Oryol {

using namespace _priv;

//------------------------------------------------------------------------------
GfxResourceContainer::GfxResourceContainer() :
runLoopId(RunLoop::InvalidId),
renderer(nullptr),
displayMgr(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
void
GfxResourceContainer::setup(const GfxSetup& setup, class renderer* rendr, class displayMgr* dspMgr) {
    o_assert(!this->isValid());
    o_assert(nullptr != rendr);
    o_assert(nullptr != dspMgr);
    
    this->renderer = rendr;
    this->displayMgr = dspMgr;
    
    this->meshFactory.Setup(this->renderer, &this->meshPool);
    this->meshPool.Setup(&this->meshFactory, setup.PoolSize(ResourceType::Mesh), setup.Throttling(ResourceType::Mesh), 'MESH');
    this->shaderFactory.Setup();
    this->shaderPool.Setup(&this->shaderFactory, setup.PoolSize(ResourceType::Shader), 0, 'SHDR');
    this->programBundleFactory.Setup(this->renderer, &this->shaderPool, &this->shaderFactory);
    this->programBundlePool.Setup(&this->programBundleFactory, setup.PoolSize(ResourceType::ProgramBundle), 0, 'PRGB');
    this->textureFactory.Setup(this->renderer, this->displayMgr, &this->texturePool);
    this->texturePool.Setup(&this->textureFactory, setup.PoolSize(ResourceType::Texture), setup.Throttling(ResourceType::Texture), 'TXTR');
    this->drawStateFactory.Setup(&this->meshPool, &this->programBundlePool);
    this->drawStatePool.Setup(&this->drawStateFactory, setup.PoolSize(ResourceType::DrawState), 0, 'DRWS');
    
    resourceContainerBase::setup(setup.ResourceLabelStackCapacity, setup.ResourceRegistryCapacity);
}

//------------------------------------------------------------------------------
void
GfxResourceContainer::discard() {
    o_assert_dbg(this->isValid());
    
    resourceContainerBase::discard();
    
    Core::PreRunLoop()->Remove(this->runLoopId);
    this->drawStatePool.Discard();
    this->drawStateFactory.Discard();
    this->texturePool.Discard();
    this->textureFactory.Discard();
    this->programBundlePool.Discard();
    this->programBundleFactory.Discard();
    this->shaderPool.Discard();
    this->shaderFactory.Discard();
    this->meshPool.Discard();
    this->meshFactory.Discard();
    this->renderer = nullptr;
    this->displayMgr = nullptr;
}

//------------------------------------------------------------------------------
void
GfxResourceContainer::update() {
    o_assert_dbg(this->isValid());
    
    // only call Update on pools which support asynchronous resource loading
    // FIXME: MOVE ASYNC LOADING OUT OF RESOURCE POOLS
}

//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const MeshSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(!setup.ShouldSetupFromFileAsync());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->meshPool.AllocId(this->peekLabel());
        this->meshPool.Assign(resId, setup);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const MeshSetup& setup, const Ptr<Stream>& data) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(!setup.ShouldSetupFromFileAsync());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->meshPool.AllocId(this->peekLabel());
        this->meshPool.Assign(resId, setup, data);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const TextureSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(!setup.ShouldSetupFromFileAsync());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->texturePool.AllocId(this->peekLabel());
        this->texturePool.Assign(resId, setup);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const TextureSetup& setup, const Ptr<Stream>& data) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(!setup.ShouldSetupFromFileAsync());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->texturePool.AllocId(this->peekLabel());
        this->texturePool.Assign(resId, setup, data);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const ShaderSetup& setup) {
    o_assert_dbg(this->isValid());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->shaderPool.AllocId(this->peekLabel());
        this->shaderPool.Assign(resId, setup);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const ProgramBundleSetup& setup) {
    o_assert_dbg(this->isValid());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->programBundlePool.AllocId(this->peekLabel());
        this->programBundlePool.Assign(resId, setup);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const DrawStateSetup& setup) {
    o_assert_dbg(this->isValid());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->drawStatePool.AllocId(this->peekLabel());
        this->drawStatePool.Assign(resId, setup);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
void
GfxResourceContainer::Destroy(uint8 label) {
    o_assert_dbg(this->isValid());
    this->registry.Remove(label);
    this->texturePool.UnassignByLabel(label);
    this->meshPool.UnassignByLabel(label);
    this->shaderPool.UnassignByLabel(label);
    this->programBundlePool.UnassignByLabel(label);
    this->drawStatePool.UnassignByLabel(label);
}
    
//------------------------------------------------------------------------------
ResourceState::Code
GfxResourceContainer::QueryState(const Id& resId) const {
    o_assert_dbg(this->isValid());
    switch (resId.Type()) {
        case ResourceType::Texture:
            return this->texturePool.QueryState(resId);
        case ResourceType::Mesh:
            return this->meshPool.QueryState(resId);
        case ResourceType::Shader:
            return this->shaderPool.QueryState(resId);
        case ResourceType::ProgramBundle:
            return this->programBundlePool.QueryState(resId);
        case ResourceType::ConstantBlock:
            o_assert2(false, "FIXME!!!\n");
            break;
        case ResourceType::DrawState:
            return this->drawStatePool.QueryState(resId);
        default:
            o_assert(false);
            break;
    }
    return ResourceState::InvalidState;
}

} // namespace Oryol