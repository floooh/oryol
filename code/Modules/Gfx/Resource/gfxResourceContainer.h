#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::gfxResourceContainer
    @ingroup _priv
    @brief resource container implementation of the Gfx module
*/
#include "Core/Core.h"
#include "Core/RunLoop.h"
#include "Core/Threading/RWLock.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/KeyValuePair.h"
#include "IO/IOProtocol.h"
#include "Resource/Core/resourceContainerBase.h"
#include "Resource/Core/SetupAndStream.h"
#include "Resource/ResourceInfo.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Resource/meshPool.h"
#include "Gfx/Resource/meshFactory.h"
#include "Gfx/Resource/shaderPool.h"
#include "Gfx/Resource/shaderFactory.h"
#include "Gfx/Resource/programBundlePool.h"
#include "Gfx/Resource/programBundleFactory.h"
#include "Gfx/Resource/texturePool.h"
#include "Gfx/Resource/textureFactory.h"
#include "Gfx/Resource/drawStatePool.h"
#include "Gfx/Resource/drawStateFactory.h"
#include "Gfx/Resource/MeshLoaderBase.h"
#include "Gfx/Resource/TextureLoaderBase.h"

namespace Oryol {

namespace _priv {
    
class renderer;
class displayMgr;

class gfxResourceContainer : public resourceContainerBase {
public:
    /// constructor
    gfxResourceContainer();

    /// setup the resource container
    void setup(const GfxSetup& setup, _priv::renderer* rendr, _priv::displayMgr* dspMgr);
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
    _priv::mesh* lookupMesh(const Id& resId);
    /// lookup program bundle object
    _priv::programBundle* lookupProgramBundle(const Id& resId);
    /// lookup texture object
    _priv::texture* lookupTexture(const Id& resId);
    /// lookup draw-state object
    _priv::drawState* lookupDrawState(const Id& resId);

    /// per-frame update (update resource pools and pending loader
    void update();
    
    class _priv::renderer* renderer;
    class _priv::displayMgr* displayMgr;
    class _priv::meshFactory meshFactory;
    class _priv::shaderFactory shaderFactory;
    class _priv::programBundleFactory programBundleFactory;
    class _priv::textureFactory textureFactory;
    class _priv::drawStateFactory drawStateFactory;
    class _priv::meshPool meshPool;
    class _priv::shaderPool shaderPool;
    class _priv::programBundlePool programBundlePool;
    class _priv::texturePool texturePool;
    class _priv::drawStatePool drawStatePool;
    RunLoop::Id runLoopId;
    Array<Ptr<ResourceLoader>> pendingLoaders;
};

//------------------------------------------------------------------------------
inline _priv::mesh*
gfxResourceContainer::lookupMesh(const Id& resId) {
    o_assert_dbg(this->valid);
    o_assert_dbg(Core::IsMainThread());
    return this->meshPool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline _priv::programBundle*
gfxResourceContainer::lookupProgramBundle(const Id& resId) {
    o_assert_dbg(this->valid);
    o_assert_dbg(Core::IsMainThread());
    return this->programBundlePool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline _priv::texture*
gfxResourceContainer::lookupTexture(const Id& resId) {
    o_assert_dbg(this->valid);
    o_assert_dbg(Core::IsMainThread());
    return this->texturePool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline _priv::drawState*
gfxResourceContainer::lookupDrawState(const Id& resId) {
    o_assert_dbg(this->valid);
    o_assert_dbg(Core::IsMainThread());
    return this->drawStatePool.Lookup(resId);
}

} // namespace _priv
} // namespace Oryol
