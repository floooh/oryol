#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourceBase
    @ingroup Resource
    @brief resource base class
    
    This is the generic Oryol resource base class. Resources usually manage
    some external data object (like textures, vertices, animations, ...)
    which can be shared, can be created synchronously or asynchronously.
    Resources generally live in ResourcePools, and only a maximum number
    of resources of the same type can exist at one time. The creation
    of a resource is controlled by a matching Setup object, which holds
    all information required to create a resource object. A copy of the
    setup object is stored in the resource object, so that the resource
    can be destroyed and re-created if needed.
*/
#include "Core/Assertion.h"
#include "Resource/Id.h"
#include "Resource/ResourceState.h"

namespace Oryol {
    
template<class SETUP> class ResourceBase {
public:
    /// the resource id
    class Id Id;
    /// current resource state
    ResourceState::Code State = ResourceState::Initial;
    /// frame count of last state change
    int StateStartFrame = 0;
    /// the setup object
    SETUP Setup;
    
    /// clear the resource (does not reset Id or state)
    void Clear();
};

//------------------------------------------------------------------------------
template<class SETUP> void
ResourceBase<SETUP>::Clear() {
    this->Setup = SETUP();
}

} // namespace Oryol

 