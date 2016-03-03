#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::gfxResourceContainerBase
    @ingroup _priv
    @brief resource container base implementation of the Gfx module
*/
#include "Core/Core.h"
#include "Core/RunLoop.h"
#include "Core/Threading/RWLock.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/KeyValuePair.h"
#include "IO/IOProtocol.h"
#include "Resource/Core/resourceContainerBase.h"
#include "Resource/ResourceInfo.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Resource/resourcePools.h"
#include "Gfx/Resource/meshFactory.h"
#include "Gfx/Resource/shaderFactory.h"
#include "Gfx/Resource/textureFactory.h"
#include "Gfx/Resource/drawStateFactory.h"
#include "Gfx/Resource/MeshLoaderBase.h"
#include "Gfx/Resource/TextureLoaderBase.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {
    
class renderer;
class displayMgr;

class gfxResourceContainerBase : public resourceContainerBase {
public:
    /// constructor
    gfxResourceContainerBase();

    /// setup the resource container
    void setup(const GfxSetup& setup, const gfxPointers& ptrs);
    /// discard the resource manager
    void discard();
    
    /// create a resource object
    template<class SETUP> Id Create(const SETUP& setup);
    /// create a resource object with data
    template<class SETUP> Id Create(const SETUP& setup, const void* data, int32 size);
    /// asynchronously load resource object
    Id Load(const Ptr<ResourceLoader>& loader);
    /// query number of free slots for resource type
    int32 QueryFreeSlots(GfxResourceType::Code resourceType) const;
    /// query resource info (fast)
    ResourceInfo QueryResourceInfo(const Id& id) const;
    /// query resource pool info (slow)
    ResourcePoolInfo QueryPoolInfo(GfxResourceType::Code resType) const;
    /// destroy resources by label
    void Destroy(ResourceLabel label);
    
    /// prepare async creation (usually called at start of async Load)
    template<class SETUP> Id prepareAsync(const SETUP& setup);
    /// setup async resource (usually called during async Load)
    template<class SETUP> ResourceState::Code initAsync(const Id& resId, const SETUP& setup, const void* data, int32 size);
    /// notify resource container that async creation had failed
    ResourceState::Code failedAsync(const Id& resId);

    /// lookup mesh object
    mesh* lookupMesh(const Id& resId);
    /// lookup shader object
    shader* lookupShader(const Id& resId);
    /// lookup texture object
    texture* lookupTexture(const Id& resId);
    /// lookup draw-state object
    drawState* lookupDrawState(const Id& resId);

    /// per-frame update (update resource pools and pending loaders)
    void update();

    gfxPointers pointers;
    class meshFactory meshFactory;
    class shaderFactory shaderFactory;
    class textureFactory textureFactory;
    class drawStateFactory drawStateFactory;
    class meshPool meshPool;
    class shaderPool shaderPool;
    class texturePool texturePool;
    class drawStatePool drawStatePool;
    RunLoop::Id runLoopId;
    Array<Ptr<ResourceLoader>> pendingLoaders;
};

//------------------------------------------------------------------------------
inline mesh*
gfxResourceContainerBase::lookupMesh(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->meshPool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline shader*
gfxResourceContainerBase::lookupShader(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->shaderPool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline texture*
gfxResourceContainerBase::lookupTexture(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->texturePool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline drawState*
gfxResourceContainerBase::lookupDrawState(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->drawStatePool.Lookup(resId);
}

} // namespace _priv
} // namespace Oryol
