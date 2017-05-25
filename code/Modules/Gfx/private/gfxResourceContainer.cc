//------------------------------------------------------------------------------
//  gfxResourceContainer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "gfxResourceContainer.h"
#include "displayMgr.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
gfxResourceContainer::setup(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert(!this->IsValid());
    
    this->pointers = ptrs;
    this->pendingLoaders.Reserve(128);
    this->destroyQueue.Reserve(128);

    this->meshPool.Setup(GfxResourceType::Mesh, setup.ResourcePoolSize[GfxResourceType::Mesh]);
    this->shaderPool.Setup(GfxResourceType::Shader, setup.ResourcePoolSize[GfxResourceType::Shader]);
    this->texturePool.Setup(GfxResourceType::Texture, setup.ResourcePoolSize[GfxResourceType::Texture]);
    this->pipelinePool.Setup(GfxResourceType::Pipeline, setup.ResourcePoolSize[GfxResourceType::Pipeline]);
    this->renderPassPool.Setup(GfxResourceType::RenderPass, setup.ResourcePoolSize[GfxResourceType::RenderPass]);
    this->factory.setup(this->pointers);
    this->runLoopId = Core::PostRunLoop()->Add([this]() {
        this->update();
    });
    
    ResourceContainerBase::Setup(setup.ResourceLabelStackCapacity, setup.ResourceRegistryCapacity);
}

//------------------------------------------------------------------------------
void
gfxResourceContainer::discard() {
    o_assert_dbg(this->IsValid());
    
    Core::PostRunLoop()->Remove(this->runLoopId);
    for (const auto& loader : this->pendingLoaders) {
        loader->Cancel();
    }
    this->pendingLoaders.Clear();
    
    ResourceContainerBase::Discard();

    this->renderPassPool.Discard();
    this->pipelinePool.Discard();
    this->texturePool.Discard();
    this->shaderPool.Discard();
    this->meshPool.Discard();
    this->factory.discard();
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
template<> Id
gfxResourceContainer::Create(const MeshSetup& setup, const void* data, int size) {
    o_assert_dbg(this->IsValid());
    o_assert_dbg(!setup.ShouldSetupFromFile());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->meshPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->PeekLabel());
        mesh& res = this->meshPool.Assign(resId, ResourceState::Setup);
        res.Setup = setup;
        const ResourceState::Code newState = this->factory.initMesh(res, data, size);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->meshPool.UpdateState(resId, newState);
    }
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
gfxResourceContainer::Create(const TextureSetup& setup, const void* data, int size) {
    o_assert_dbg(this->IsValid());
    o_assert_dbg(!setup.ShouldSetupFromFile());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->texturePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->PeekLabel());
        texture& res = this->texturePool.Assign(resId, ResourceState::Setup);
        res.Setup = setup;
        const ResourceState::Code newState = this->factory.initTexture(res, data, size);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->texturePool.UpdateState(resId, newState);
    }
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
gfxResourceContainer::prepareAsync(const MeshSetup& setup) {
    o_assert_dbg(this->IsValid());
    
    Id resId = this->meshPool.AllocId();
    this->registry.Add(setup.Locator, resId, this->PeekLabel());
    mesh& res = this->meshPool.Assign(resId, ResourceState::Pending);
    res.Setup = setup;
    return resId;
}

//------------------------------------------------------------------------------
template<> ResourceState::Code 
gfxResourceContainer::initAsync(const Id& resId, const MeshSetup& setup, const void* data, int size) {
    o_assert_dbg(this->IsValid());
    
    // the prepared resource may have been destroyed while it was loading
    if (this->meshPool.Contains(resId)) {
        mesh& res = this->meshPool.Assign(resId, ResourceState::Pending);
        res.Setup = setup;
        const ResourceState::Code newState = this->factory.initMesh(res, data, size);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->meshPool.UpdateState(resId, newState);
        return newState;
    }
    else {
        // the prepared mesh object was destroyed before it was loaded
        o_warn("gfxResourceContainer::initAsync(): resource destroyed before initAsync (type: %d, slot: %d!)\n",
            resId.Type, resId.SlotIndex);
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
template<> Id
gfxResourceContainer::prepareAsync(const TextureSetup& setup) {
    o_assert_dbg(this->IsValid());
    
    Id resId = this->texturePool.AllocId();
    this->registry.Add(setup.Locator, resId, this->PeekLabel());
    texture& res = this->texturePool.Assign(resId, ResourceState::Pending);
    res.Setup = setup;
    return resId;
}

//------------------------------------------------------------------------------
template<> ResourceState::Code 
gfxResourceContainer::initAsync(const Id& resId, const TextureSetup& setup, const void* data, int size) {
    o_assert_dbg(this->IsValid());
    
    // the prepared resource may have been destroyed while it was loading
    if (this->texturePool.Contains(resId)) {
        texture& res = this->texturePool.Assign(resId, ResourceState::Pending);
        res.Setup = setup;
        const ResourceState::Code newState = this->factory.initTexture(res, data, size);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->texturePool.UpdateState(resId, newState);
        return newState;
    }
    else {
        // the prepared texture object was destroyed before it was loaded
        o_warn("gfxResourceContainer::initAsync(): resource destroyed before initAsync (type: %d, slot: %d!)\n",
            resId.Type, resId.SlotIndex);
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
gfxResourceContainer::failedAsync(const Id& resId) {
    o_assert_dbg(this->IsValid());
    
    switch (resId.Type) {
        case GfxResourceType::Mesh:
            // the prepared resource may have been destroyed while it was loading
            if (this->meshPool.Contains(resId)) {
                this->meshPool.UpdateState(resId, ResourceState::Failed);
                return ResourceState::Failed;
            }
            break;
            
        case GfxResourceType::Texture:
            // the prepared resource may have been destroyed while it was loading
            if (this->texturePool.Contains(resId)) {
                this->texturePool.UpdateState(resId, ResourceState::Failed);
                return ResourceState::Failed;
            }
            break;
            
        default:
            o_error("Invalid resource type for async creation!");
            break;
    }
    // fallthrough: resource was already destroyed while still loading
    return ResourceState::InvalidState;
}


//------------------------------------------------------------------------------
template<> Id
gfxResourceContainer::Create(const ShaderSetup& setup, const void* /*data*/, int /*size*/) {
    o_assert_dbg(this->IsValid());
    
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->shaderPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->PeekLabel());
        shader& res = this->shaderPool.Assign(resId, ResourceState::Setup);
        res.Setup = setup;
        const ResourceState::Code newState = this->factory.initShader(res);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->shaderPool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
gfxResourceContainer::Create(const PipelineSetup& setup, const void* /*data*/, int /*size*/) {
    o_assert_dbg(this->IsValid());
    
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->pipelinePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->PeekLabel());
        pipeline& res = this->pipelinePool.Assign(resId, ResourceState::Setup);
        res.Setup = setup;
        const ResourceState::Code newState = this->factory.initPipeline(res);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));        
        this->pipelinePool.UpdateState(resId, newState);
    }
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
gfxResourceContainer::Create(const PassSetup& setup, const void* /*data*/, int /*size*/) {
    o_assert_dbg(this->IsValid());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->renderPassPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->PeekLabel());
        renderPass& res = this->renderPassPool.Assign(resId, ResourceState::Setup);
        res.Setup = setup;
        const ResourceState::Code newState = this->factory.initRenderPass(res);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->renderPassPool.UpdateState(resId, newState);
    }
    return resId;
}

//------------------------------------------------------------------------------
Id
gfxResourceContainer::Load(const Ptr<ResourceLoader>& loader) {
    o_assert_dbg(this->IsValid());

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
gfxResourceContainer::DestroyDeferred(const ResourceLabel& label) {
    o_assert_dbg(this->IsValid());
    Array<Id> ids = this->registry.Remove(label);
    if (ids.Size() > 0) {
        this->destroyQueue.Reserve(ids.Size());
        for (const Id& id : ids) {
            this->destroyQueue.Add(id);
        }
    }
}

//------------------------------------------------------------------------------
void
gfxResourceContainer::GarbageCollect() {
    for (const Id& id : this->destroyQueue) {
        this->destroyResource(id);
    }
    this->destroyQueue.Clear();
    this->factory.garbageCollect();
}

//------------------------------------------------------------------------------
void
gfxResourceContainer::destroyResource(const Id& id) {
    switch (id.Type) {
        case GfxResourceType::Texture:
        {
            if (ResourceState::Valid == this->texturePool.QueryState(id)) {
                texture* tex = this->texturePool.Lookup(id);
                if (tex) {
                    this->factory.destroyTexture(*tex);
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
                    this->factory.destroyMesh(*msh);
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
                    this->factory.destroyShader(*shd);
                }
            }
            this->shaderPool.Unassign(id);
        }
        break;
            
        case GfxResourceType::Pipeline:
        {
            if (ResourceState::Valid == this->pipelinePool.QueryState(id)) {
                pipeline* pip = this->pipelinePool.Lookup(id);
                if (pip) {
                    this->factory.destroyPipeline(*pip);
                }
            }
            this->pipelinePool.Unassign(id);
        }
        break;

        case GfxResourceType::RenderPass:
        {
            if (ResourceState::Valid == this->renderPassPool.QueryState(id)) {
                renderPass* rp = this->renderPassPool.Lookup(id);
                if (rp) {
                    this->factory.destroyRenderPass(*rp);
                }
            }
            this->renderPassPool.Unassign(id);
        }
        break;

        default:
            o_assert(false);
            break;
    }
}

//------------------------------------------------------------------------------
void
gfxResourceContainer::Destroy(const ResourceLabel& label) {
    o_assert_dbg(this->IsValid());
    Array<Id> ids = this->registry.Remove(label);
    for (const Id& id : ids) {
        this->destroyResource(id);
    }
}
    
//------------------------------------------------------------------------------
void
gfxResourceContainer::update() {
    o_assert_dbg(this->IsValid());
    
    /// call update method on resource pools (this is cheap)
    this->meshPool.Update();
    this->shaderPool.Update();
    this->texturePool.Update();
    this->pipelinePool.Update();

    // trigger loaders, and remove from pending array if finished
    for (int i = this->pendingLoaders.Size() - 1; i >= 0; i--) {
        const auto& loader = this->pendingLoaders[i];
        ResourceState::Code state = loader->Continue();
        if (ResourceState::Pending != state) {
            this->pendingLoaders.Erase(i);
        }
    }
}

//------------------------------------------------------------------------------
ResourceInfo
gfxResourceContainer::QueryResourceInfo(const Id& resId) const {
    o_assert_dbg(this->IsValid());
    
    switch (resId.Type) {
        case GfxResourceType::Texture:
            return this->texturePool.QueryResourceInfo(resId);
        case GfxResourceType::Mesh:
            return this->meshPool.QueryResourceInfo(resId);
        case GfxResourceType::Shader:
            return this->shaderPool.QueryResourceInfo(resId);
        case GfxResourceType::Pipeline:
            return this->pipelinePool.QueryResourceInfo(resId);
        case GfxResourceType::RenderPass:
            return this->renderPassPool.QueryResourceInfo(resId);
        default:
            o_assert(false);
            return ResourceInfo();
    }
}

//------------------------------------------------------------------------------
ResourcePoolInfo
gfxResourceContainer::QueryPoolInfo(GfxResourceType::Code resType) const {
    o_assert_dbg(this->IsValid());
    
    switch (resType) {
        case GfxResourceType::Texture:
            return this->texturePool.QueryPoolInfo();
        case GfxResourceType::Mesh:
            return this->meshPool.QueryPoolInfo();
        case GfxResourceType::Shader:
            return this->shaderPool.QueryPoolInfo();
        case GfxResourceType::Pipeline:
            return this->pipelinePool.QueryPoolInfo();
        case GfxResourceType::RenderPass:
            return this->renderPassPool.QueryPoolInfo();
        default:
            o_assert(false);
            return ResourcePoolInfo();
    }
}

//------------------------------------------------------------------------------
int
gfxResourceContainer::QueryFreeSlots(GfxResourceType::Code resourceType) const {
    o_assert_dbg(this->IsValid());

    switch (resourceType) {
        case GfxResourceType::Texture:
            return this->texturePool.GetNumFreeSlots();
        case GfxResourceType::Mesh:
            return this->meshPool.GetNumFreeSlots();
        case GfxResourceType::Shader:
            return this->shaderPool.GetNumFreeSlots();
        case GfxResourceType::Pipeline:
            return this->pipelinePool.GetNumFreeSlots();
        case GfxResourceType::RenderPass:
            return this->renderPassPool.GetNumFreeSlots();
        default:
            o_assert(false);
            return 0;
    }
}

} // namespace _priv
} // namespace Oryol
