#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::GfxResourceContainer
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
#include "Resource/Core/ResourceStreamTarget.h"
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
}
    
class GfxResourceContainer : public resourceContainerBase {
public:
    /// constructor
    GfxResourceContainer();
    
    /// per-frame update for pending/deferred resource creation and destruction
    void UpdatePending();

    /// create a resource object
    template<class SETUP> Id Create(const SETUP& setup);
    /// create a resource object with data
    template<class SETUP> Id Create(const SETUP& setup, const Ptr<Stream>& stream);
    /// create a resource object with data
    template<class SETUP> Id Create(const SetupAndStream<SETUP>& setupAndStream);
    /// asynchronously load resource object
    template<class SETUP, class LOADER> Id Load(const SETUP& setup, int32 ioLane, std::function<Ptr<LOADER>()> loader);
    /// query current resource state
    ResourceState::Code QueryState(const Id& id) const;
    /// destroy resources by label
    void Destroy(uint8 label);
    
    /// initialize resource from a different thread (usually called during Load())
    template<class SETUP> void initResourceFromThread(int32 threadIndex, const Id& resId, const SetupAndStream<SETUP>& setupAndStream);
    /// notify Gfx that threaded resource creation has failed
    void failResourceFromThread(int32 threadIndex, const Id& resId);
    
private:
    friend class Gfx;

    /// setup the resource container
    void setup(const GfxSetup& setup, _priv::renderer* rendr, _priv::displayMgr* dspMgr);
    /// discard the resource manager
    void discard();
    /// lookup mesh object
    _priv::mesh* lookupMesh(const Id& resId);
    /// lookup program bundle object
    _priv::programBundle* lookupProgramBundle(const Id& resId);
    /// lookup texture object
    _priv::texture* lookupTexture(const Id& resId);
    /// lookup draw-state object
    _priv::drawState* lookupDrawState(const Id& resId);
    
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
    Array<Id> pendingDestroys;
    RWLock pendingCreateLock;
    Array<KeyValuePair<Id, ResourceState::Code>> pendingCreates;
};

//------------------------------------------------------------------------------
template<class SETUP> Id
GfxResourceContainer::Create(const SETUP& setup, const Ptr<Stream>& stream) {
    o_assert_dbg(this->valid);
    o_assert_dbg(Core::IsMainThread());
    return this->Create(SetupAndStream<SETUP>(setup, stream));
}

//------------------------------------------------------------------------------
inline _priv::mesh*
GfxResourceContainer::lookupMesh(const Id& resId) {
    o_assert_dbg(this->valid);
    o_assert_dbg(Core::IsMainThread());
    return this->meshPool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline _priv::programBundle*
GfxResourceContainer::lookupProgramBundle(const Id& resId) {
    o_assert_dbg(this->valid);
    o_assert_dbg(Core::IsMainThread());
    return this->programBundlePool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline _priv::texture*
GfxResourceContainer::lookupTexture(const Id& resId) {
    o_assert_dbg(this->valid);
    o_assert_dbg(Core::IsMainThread());
    return this->texturePool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline _priv::drawState*
GfxResourceContainer::lookupDrawState(const Id& resId) {
    o_assert_dbg(this->valid);
    o_assert_dbg(Core::IsMainThread());
    return this->drawStatePool.Lookup(resId);
}
    
} // namespace Oryol