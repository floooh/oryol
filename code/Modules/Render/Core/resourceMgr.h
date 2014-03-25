#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::resourceMgr
    @brief rendering resource manager
    
    The resource manager handles creation, sharing and destruction of
    rendering resources.
*/
#include "Render/Setup/RenderSetup.h"
#include "Render/Core/meshPool.h"
#include "Render/Core/shaderPool.h"
#include "Render/Core/programBundlePool.h"
#include "Resource/Registry.h"
#include "Resource/Pool.h"

namespace Oryol {
namespace Render {

class stateWrapper;
class meshBaseLoader;
    
class resourceMgr {
public:
    /// constructor
    resourceMgr();
    /// destructor
    ~resourceMgr();
    
    /// attach a resource loader
    void AttachLoader(const Core::Ptr<meshLoaderBase>& loader);
    
    /// setup the resource manager
    void Setup(const RenderSetup& setup, stateWrapper* stWrapper);
    /// discard the resource manager
    void Discard();
    /// return true if the resource manager has been setup
    bool IsValid() const;
    /// per-frame update
    void Update();
    
    /// create a resource, or return existing resource
    template<class SETUP> Resource::Id CreateResource(const SETUP& setup);
    /// create a resource with data stream, or return existing resource
    template<class SETUP> Resource::Id CreateResource(const SETUP& setup, const Core::Ptr<IO::Stream>& data);
    /// lookup a resource by resource locator (incremenets use-count of resource!)
    Resource::Id LookupResource(const Resource::Locator& locator);
    /// discard a resource (decrement use-count, free resource if use-count is 0)
    void DiscardResource(const Resource::Id& resId);
    /// get the loading state of a resource
    Resource::State::Code QueryResourceState(const Resource::Id& resId);
    
    /// lookup mesh object
    mesh* LookupMesh(const Resource::Id& resId);
    /// lookup program bundle object
    programBundle* LookupProgramBundle(const Resource::Id& resId);

private:
    bool isValid;
    class stateWrapper* stateWrapper;
    Resource::Registry resourceRegistry;
    Core::Array<Resource::Id> removedIds;
    class meshFactory meshFactory;
    class shaderFactory shaderFactory;
    class programBundleFactory programBundleFactory;
    class meshPool meshPool;
    class shaderPool shaderPool;
    class programBundlePool programBundlePool;
};

//------------------------------------------------------------------------------
inline mesh*
resourceMgr::LookupMesh(const Resource::Id& resId) {
    o_assert_dbg(this->isValid);
    return this->meshPool.Lookup(resId);
}

//------------------------------------------------------------------------------
inline programBundle*
resourceMgr::LookupProgramBundle(const Resource::Id& resId) {
    o_assert_dbg(this->isValid);
    return this->programBundlePool.Lookup(resId);
}

} // namespace Render
} // namespace Oryol
 