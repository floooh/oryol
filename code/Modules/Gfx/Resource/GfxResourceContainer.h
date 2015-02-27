#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::GfxResourceContainer
    @brief resource container implementation of the Gfx module
*/
#include "Resource/Core/resourceContainerBase.h"
#include "Resource/Core/SetupAndStream.h"
#include "Resource/Core/ResourceStreamTarget.h"
#include "Core/RunLoop.h"
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

    /// create a resource object
    template<class SETUP> Id Create(const SETUP& setup);
    /// create a resource object with data
    template<class SETUP> Id Create(const SETUP& setup, const Ptr<Stream>& stream);
    /// create a resource object with data
    template<class SETUP> Id Create(const SetupAndStream<SETUP>& setupAndStream);
    /// streaming-load resource object
    template<class SETUP, class LOADER> Id Load(const SETUP& setup, std::function<Ptr<LOADER>()> loader);
    /// query current resource state
    ResourceState::Code QueryState(const Id& id) const;
    /// destroy resources by label
    void Destroy(uint8 label);
    
    /// begin streaming resource setup, usually called by loaders (can be called from other thread)
    template<class SETUP> ResourceStreamTarget BeginStreaming(const Id& id, const SETUP& setup);
    /// finish streaming resource setup, usually called by loaders (can be called from other thread)
    void EndStreaming(const Id& id);
    
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
};

//------------------------------------------------------------------------------
template<class SETUP> Id
GfxResourceContainer::Create(const SETUP& setup, const Ptr<Stream>& stream) {
    return this->Create(SetupAndStream<SETUP>(setup, stream));
}

//------------------------------------------------------------------------------
inline _priv::mesh*
GfxResourceContainer::lookupMesh(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->meshPool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline _priv::programBundle*
GfxResourceContainer::lookupProgramBundle(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->programBundlePool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline _priv::texture*
GfxResourceContainer::lookupTexture(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->texturePool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline _priv::drawState*
GfxResourceContainer::lookupDrawState(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->drawStatePool.Lookup(resId);
}
    
} // namespace Oryol