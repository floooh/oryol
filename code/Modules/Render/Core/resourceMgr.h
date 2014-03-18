#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::resourceMgr
    @brief rendering resource manager
    
    The resource manager handles creation, sharing and destruction of
    rendering resources.
*/
#include "Render/Setup/RenderSetup.h"
#include "Render/Core/meshFactory.h"
#include "Resource/Registry.h"
#include "Resource/Pool.h"
#include "Render/Core/mesh.h"

namespace Oryol {
namespace Render {

class stateWrapper;
    
class resourceMgr {
public:
    /// constructor
    resourceMgr();
    /// destructor
    ~resourceMgr();
    
    /// attach a resource loader
    template<class LOADER> void AttachLoader(const Core::Ptr<LOADER>& loader);
    
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
    Resource::State::Code QueryResourceState(const Resource::Id& resId) const;

private:
    bool isValid;
    class stateWrapper* stateWrapper;
    Resource::Registry resourceRegistry;
    Core::Array<Resource::Id> removedIds;
    class meshFactory meshFactory;
    Resource::Pool<mesh, MeshSetup, class meshFactory> meshPool;
};
    
} // namespace Render
} // namespace Oryol
 