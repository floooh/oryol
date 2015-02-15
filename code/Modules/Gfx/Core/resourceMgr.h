#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::resourceMgr
    @ingroup _priv
    @brief rendering resource manager
    
    The resource manager handles creation, sharing and destruction of
    rendering resources.
*/
#include "Core/RunLoop.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Core/meshPool.h"
#include "Gfx/Core/shaderPool.h"
#include "Gfx/Core/programBundlePool.h"
#include "Gfx/Core/texturePool.h"
#include "Gfx/Core/drawStatePool.h"

namespace Oryol {
namespace _priv {

class renderer;
class displayMgr;
class meshBaseLoader;
    
class resourceMgr {
public:
    /// constructor
    resourceMgr();
    /// destructor
    ~resourceMgr();
    
    /// attach mesh loader
    void AttachLoader(const Ptr<loaderBase>& loader);
    
    /// setup the resource manager
    void Setup(const GfxSetup& setup, renderer* rendr, displayMgr* dspMgr);
    /// discard the resource manager
    void Discard();
    /// return true if the resource manager has been setup
    bool IsValid() const;
    /// per-frame update
    void Update();
    
    /// create a resource, or return existing resource
    template<class SETUP> Id CreateResource(const SETUP& setup);
    /// create a resource with data stream, or return existing resource
    template<class SETUP> Id CreateResource(const SETUP& setup, const Ptr<Stream>& data);
    /// discard a resource
    void DiscardResource(const Id& resId);
    /// get the loading state of a resource
    ResourceState::Code QueryResourceState(const Id& resId);
    
    /// lookup mesh object
    mesh* LookupMesh(const Id& resId);
    /// lookup program bundle object
    programBundle* LookupProgramBundle(const Id& resId);
    /// lookup texture object
    texture* LookupTexture(const Id& resId);
    /// lookup draw-state object
    drawState* LookupDrawState(const Id& resId);
    
private:
    bool isValid;
    RunLoop::Id runLoopId;
    class renderer* renderer;
    class displayMgr* displayMgr;
    class meshFactory meshFactory;
    class shaderFactory shaderFactory;
    class programBundleFactory programBundleFactory;
    class textureFactory textureFactory;
    class drawStateFactory drawStateFactory;
    class meshPool meshPool;
    class shaderPool shaderPool;
    class programBundlePool programBundlePool;
    class texturePool texturePool;
    class drawStatePool drawStatePool;
};

//------------------------------------------------------------------------------
inline mesh*
resourceMgr::LookupMesh(const Id& resId) {
    o_assert_dbg(this->isValid);
    return this->meshPool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline programBundle*
resourceMgr::LookupProgramBundle(const Id& resId) {
    o_assert_dbg(this->isValid);
    return this->programBundlePool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline texture*
resourceMgr::LookupTexture(const Id& resId) {
    o_assert_dbg(this->isValid);
    return this->texturePool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline drawState*
resourceMgr::LookupDrawState(const Id& resId) {
    o_assert_dbg(this->isValid);
    return this->drawStatePool.Lookup(resId);
}

} // namespace _priv
} // namespace Oryol
 