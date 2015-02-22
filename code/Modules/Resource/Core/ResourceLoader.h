#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourceLoader
    @ingroup Resource
    @brief base class for resource loaders
    
    The ResourceLoader base class only implements a minimal interface
    of virtual functions. How the actual resource loading happens
    is entirely up to a specific implementation in subclasses.
    
    Usually a subclass would implement a Supports() method to indicate
    what resource types the loader supports, an Accepts() method to
    check whether a specific resource loading request is accepted,
    and a Load() to actually perform the load.
    
    @see GfxResourceContainer, MeshLoader, TextureLoader,
*/
#include "Core/RefCounted.h"

namespace Oryol {

class ResourceLoader : public RefCounted {
    OryolClassDecl(ResourceLoader);
public:
    /// constructor
    ResourceLoader();
    /// destructor
    virtual ~ResourceLoader();
    
    /// called when attached to resource container
    virtual void OnAttach();
    /// called when detached from resource container
    virtual void OnDetach();
    /// return true if attached
    bool IsAttached() const;
    
protected:
    bool isAttached;
};

} // namespace Oryol