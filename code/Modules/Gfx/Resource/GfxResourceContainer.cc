//------------------------------------------------------------------------------
//  GfxResourceContainer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "IO/IO.h"
#include "GfxResourceContainer.h"
#include "Gfx/Core/displayMgr.h"

namespace Oryol {

using namespace _priv;

//------------------------------------------------------------------------------
GfxResourceContainer::GfxResourceContainer() :
renderer(nullptr),
displayMgr(nullptr),
runLoopId(RunLoop::InvalidId) {
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
    this->meshPool.Setup(GfxResourceType::Mesh, setup.PoolSize(GfxResourceType::Mesh));
    this->shaderFactory.Setup();
    this->shaderPool.Setup(GfxResourceType::Shader, setup.PoolSize(GfxResourceType::Shader));
    this->programBundleFactory.Setup(this->renderer, &this->shaderPool, &this->shaderFactory);
    this->programBundlePool.Setup(GfxResourceType::ProgramBundle, setup.PoolSize(GfxResourceType::ProgramBundle));
    this->textureFactory.Setup(this->renderer, this->displayMgr, &this->texturePool);
    this->texturePool.Setup(GfxResourceType::Texture, setup.PoolSize(GfxResourceType::Texture));
    this->drawStateFactory.Setup(&this->meshPool, &this->programBundlePool);
    this->drawStatePool.Setup(GfxResourceType::DrawState, setup.PoolSize(GfxResourceType::DrawState));
    
    this->runLoopId = Core::PostRunLoop()->Add([this]() {
        this->update();
    });
    
    resourceContainerBase::setup(setup.ResourceLabelStackCapacity, setup.ResourceRegistryCapacity);
}

//------------------------------------------------------------------------------
void
GfxResourceContainer::discard() {
    o_assert_dbg(this->isValid());
    
    Core::PostRunLoop()->Remove(this->runLoopId);
    for (const auto& loader : this->pendingLoaders) {
        loader->Cancel();
    }
    this->pendingLoaders.Clear();
    
    resourceContainerBase::discard();

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
template<> Id
GfxResourceContainer::Create(const MeshSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    o_assert_dbg(!setup.ShouldSetupFromFile());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->meshPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        mesh& res = this->meshPool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->meshFactory.SetupResource(res);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->meshPool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const SetupAndStream<MeshSetup>& setupAndStream) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    const MeshSetup& setup = setupAndStream.Setup;
    const Ptr<Stream>& data = setupAndStream.Stream;
    o_assert_dbg(!setup.ShouldSetupFromFile());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->meshPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        mesh& res = this->meshPool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->meshFactory.SetupResource(res, data);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->meshPool.UpdateState(resId, newState);
    }
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const TextureSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    o_assert_dbg(!setup.ShouldSetupFromFile());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->texturePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        texture& res = this->texturePool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->textureFactory.SetupResource(res);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->texturePool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const SetupAndStream<TextureSetup>& setupAndStream) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());

    const TextureSetup& setup = setupAndStream.Setup;
    const Ptr<Stream>& data = setupAndStream.Stream;
    o_assert_dbg(!setup.ShouldSetupFromFile());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->texturePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        texture& res = this->texturePool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->textureFactory.SetupResource(res, data);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->texturePool.UpdateState(resId, newState);
    }
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::prepareAsync(const TextureSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    Id resId = this->texturePool.AllocId();
    this->registry.Add(setup.Locator, resId, this->peekLabel());
    this->texturePool.Assign(resId, setup, ResourceState::Pending);
    return resId;
}

//------------------------------------------------------------------------------
template<> ResourceState::Code 
GfxResourceContainer::initAsync(const Id& resId, const SetupAndStream<TextureSetup>& setupAndStream) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    // the prepared resource may have been destroyed while it was loading
    if (this->texturePool.Contains(resId)) {
        const TextureSetup& setup = setupAndStream.Setup;
        const Ptr<Stream>& data = setupAndStream.Stream;
        texture& res = this->texturePool.Assign(resId, setup, ResourceState::Pending);
        const ResourceState::Code newState = this->textureFactory.SetupResource(res, data);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->texturePool.UpdateState(resId, newState);
        return newState;
    }
    else {
        // the prepared texture object was destroyed before it was loaded
        o_warn("GfxResourceContainer::initAsync(): resource destroyed before initAsync (type: %d, slot: %d!)\n",
            resId.Type(), resId.SlotIndex());
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
GfxResourceContainer::failedAsync(const Id& resId) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    // the prepared resource may have been destroyed while it was loading
    if (this->texturePool.Contains(resId)) {
        switch (resId.Type()) {
            case GfxResourceType::Mesh:
                this->meshPool.UpdateState(resId, ResourceState::Failed);
                break;
                
            case GfxResourceType::Texture:
                this->meshPool.UpdateState(resId, ResourceState::Failed);
                break;
                
            default:
                o_error("Invalid resource type for async creation!");
                break;
        }
        return ResourceState::Failed;
    }
    else {
        return ResourceState::InvalidState;
    }
}


//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const ShaderSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->shaderPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        shader& res = this->shaderPool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->shaderFactory.SetupResource(res);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->shaderPool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const ProgramBundleSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->programBundlePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        programBundle& res = this->programBundlePool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->programBundleFactory.SetupResource(res);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->programBundlePool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const DrawStateSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->drawStatePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        drawState& res = this->drawStatePool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->drawStateFactory.SetupResource(res);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));        
        this->drawStatePool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
Id
GfxResourceContainer::Load(const Ptr<ResourceLoader>& loader) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());

    Id resId = this->registry.Lookup(loader->Locator());
    if (resId.IsValid()) {
        return resId;
    }
    else {
        this->pendingLoaders.Add(loader);
        resId = loader->Start();
        return resId;
    }
}

//------------------------------------------------------------------------------
void
GfxResourceContainer::Destroy(ResourceLabel label) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    Array<Id> ids = this->registry.Remove(label);
    for (const Id& id : ids) {
        switch (id.Type()) {
            case GfxResourceType::Texture:
            {
                if (ResourceState::Valid == this->texturePool.QueryState(id)) {
                    texture* tex = this->texturePool.Lookup(id);
                    if (tex) {
                        this->textureFactory.DestroyResource(*tex);
                    }
                }
                this->texturePool.Unassign(id);
            }
            break;
                
            case GfxResourceType::Mesh:
            {
                if (ResourceState::Valid == this->meshPool.QueryState(id)) {
                    mesh* msh = this->meshPool.Lookup(id);
                    if (msh) {
                        this->meshFactory.DestroyResource(*msh);
                    }
                }
                this->meshPool.Unassign(id);
            }
            break;
                
            case GfxResourceType::Shader:
            {
                if (ResourceState::Valid == this->shaderPool.QueryState(id)) {
                    shader* shd = this->shaderPool.Lookup(id);
                    if (shd) {
                        this->shaderFactory.DestroyResource(*shd);
                    }
                }
                this->shaderPool.Unassign(id);
            }
            break;
                
            case GfxResourceType::ProgramBundle:
            {
                if (ResourceState::Valid == this->programBundlePool.QueryState(id)) {
                    programBundle* prog = this->programBundlePool.Lookup(id);
                    if (prog) {
                        this->programBundleFactory.DestroyResource(*prog);
                    }
                }
                this->programBundlePool.Unassign(id);
            }
            break;
                
            case GfxResourceType::ConstantBlock:
                o_assert2(false, "FIXME!!!\n");
                break;
                
            case GfxResourceType::DrawState:
            {
                if (ResourceState::Valid == this->drawStatePool.QueryState(id)) {
                    drawState* ds = this->drawStatePool.Lookup(id);
                    if (ds) {
                        this->drawStateFactory.DestroyResource(*ds);
                    }
                }
                this->drawStatePool.Unassign(id);
            }
            break;
                
            default:
                o_assert(false);
                break;
        }
    }
}
    
//------------------------------------------------------------------------------
void
GfxResourceContainer::update() {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    /// call update method on resource pools (this is cheap)
    this->meshPool.Update();
    this->shaderPool.Update();
    this->programBundlePool.Update();
    this->texturePool.Update();
    this->drawStatePool.Update();
    
    // trigger loaders, and remove from pending array if finished
    for (int32 i = this->pendingLoaders.Size() - 1; i >= 0; i--) {
        const auto& loader = this->pendingLoaders[i];
        ResourceState::Code state = loader->Continue();
        if (ResourceState::Pending != state) {
            this->pendingLoaders.Erase(i);
        }
    }
}

//------------------------------------------------------------------------------
ResourceInfo
GfxResourceContainer::QueryResourceInfo(const Id& resId) const {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    switch (resId.Type()) {
        case GfxResourceType::Texture:
            return this->texturePool.QueryResourceInfo(resId);
        case GfxResourceType::Mesh:
            return this->meshPool.QueryResourceInfo(resId);
        case GfxResourceType::Shader:
            return this->shaderPool.QueryResourceInfo(resId);
        case GfxResourceType::ProgramBundle:
            return this->programBundlePool.QueryResourceInfo(resId);
        case GfxResourceType::ConstantBlock:
            o_assert2(false, "FIXME!!!\n");
            return ResourceInfo();
        case GfxResourceType::DrawState:
            return this->drawStatePool.QueryResourceInfo(resId);
        default:
            o_assert(false);
            return ResourceInfo();
    }
}

//------------------------------------------------------------------------------
ResourcePoolInfo
GfxResourceContainer::QueryPoolInfo(GfxResourceType::Code resType) const {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    switch (resType) {
        case GfxResourceType::Texture:
            return this->texturePool.QueryPoolInfo();
        case GfxResourceType::Mesh:
            return this->meshPool.QueryPoolInfo();
        case GfxResourceType::Shader:
            return this->shaderPool.QueryPoolInfo();
        case GfxResourceType::ProgramBundle:
            return this->programBundlePool.QueryPoolInfo();
        case GfxResourceType::ConstantBlock:
            o_assert2(false, "FIXME!!!\n");
            return ResourcePoolInfo();
        case GfxResourceType::DrawState:
            return this->drawStatePool.QueryPoolInfo();
        default:
            o_assert(false);
            return ResourcePoolInfo();
    }
}

//------------------------------------------------------------------------------
int32
GfxResourceContainer::QueryFreeSlots(GfxResourceType::Code resourceType) const {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());

    switch (resourceType) {
        case GfxResourceType::Texture:
            return this->texturePool.GetNumFreeSlots();
        case GfxResourceType::Mesh:
            return this->meshPool.GetNumFreeSlots();
        case GfxResourceType::Shader:
            return this->shaderPool.GetNumFreeSlots();
        case GfxResourceType::ProgramBundle:
            return this->programBundlePool.GetNumFreeSlots();
        case GfxResourceType::ConstantBlock:
            o_assert2(false, "FIXME!!!\n");
            return 0;
        case GfxResourceType::DrawState:
            return this->drawStatePool.GetNumFreeSlots();
        default:
            o_assert(false);
            return 0;
    }
}

} // namespace Oryol
