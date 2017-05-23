#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::gfxResourceContainer
    @ingroup _priv
    @brief resource container base implementation of the Gfx module
*/
#include "Core/RunLoop.h"
#include "Core/Containers/Array.h"
#include "Resource/ResourceLoader.h"
#include "Resource/ResourceContainerBase.h"
#include "Resource/ResourceInfo.h"
#include "Gfx/GfxTypes.h"
#include "Gfx/private/resourcePools.h"
#include "Gfx/private/gfxFactory.h"
#include "Gfx/private/gfxPointers.h"

namespace Oryol {
namespace _priv {
    
class renderer;
class displayMgr;

class gfxResourceContainer : public ResourceContainerBase {
public:
    /// setup the resource container
    void setup(const GfxSetup& setup, const gfxPointers& ptrs);
    /// discard the resource manager
    void discard();
    
    /// create a resource object with data
    template<class SETUP> Id Create(const SETUP& setup, const void* data=nullptr, int size=0);
    /// asynchronously load resource object
    Id Load(const Ptr<ResourceLoader>& loader);
    /// query number of free slots for resource type
    int QueryFreeSlots(GfxResourceType::Code resourceType) const;
    /// query resource info (fast)
    ResourceInfo QueryResourceInfo(const Id& id) const;
    /// query resource pool info (slow)
    ResourcePoolInfo QueryPoolInfo(GfxResourceType::Code resType) const;
    /// immediately destroy resources by label
    void Destroy(const ResourceLabel& label);
    /// queue resources for destruction in GarbageCollect
    void DestroyDeferred(const ResourceLabel& label);
    /// destroy deferred resources (called from Gfx::CommitFrame)
    void GarbageCollect();
    
    /// prepare async creation (usually called at start of async Load)
    template<class SETUP> Id prepareAsync(const SETUP& setup);
    /// setup async resource (usually called during async Load)
    template<class SETUP> ResourceState::Code initAsync(const Id& resId, const SETUP& setup, const void* data, int size);
    /// notify resource container that async creation had failed
    ResourceState::Code failedAsync(const Id& resId);

    /// lookup mesh object
    mesh* lookupMesh(const Id& resId);
    /// lookup shader object
    shader* lookupShader(const Id& resId);
    /// lookup texture object
    texture* lookupTexture(const Id& resId);
    /// lookup pipeline object
    pipeline* lookupPipeline(const Id& resId);
    /// lookup render-pass object
    renderPass* lookupRenderPass(const Id& resId);

    /// per-frame update (update resource pools and pending loaders)
    void update();
    /// destroy a single resource
    void destroyResource(const Id& id);

    gfxPointers pointers;
    gfxFactory factory;
    class meshPool meshPool;
    class shaderPool shaderPool;
    class texturePool texturePool;
    class pipelinePool pipelinePool;
    class renderPassPool renderPassPool;
    RunLoop::Id runLoopId = RunLoop::InvalidId;
    Array<Ptr<ResourceLoader>> pendingLoaders;
    Array<Id> destroyQueue;
};

//------------------------------------------------------------------------------
inline mesh*
gfxResourceContainer::lookupMesh(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->meshPool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline shader*
gfxResourceContainer::lookupShader(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->shaderPool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline texture*
gfxResourceContainer::lookupTexture(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->texturePool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline pipeline*
gfxResourceContainer::lookupPipeline(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->pipelinePool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline renderPass*
gfxResourceContainer::lookupRenderPass(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->renderPassPool.Lookup(resId);
}

} // namespace _priv
} // namespace Oryol
