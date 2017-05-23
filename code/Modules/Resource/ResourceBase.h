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
    all information required to create a resource object. 
*/
#include "Core/Assertion.h"
#include "Resource/Id.h"
#include "Resource/ResourceState.h"

namespace Oryol {
    
class ResourceBase {
public:
    /// the resource id
    class Id Id;
    /// current resource state
    ResourceState::Code State = ResourceState::Initial;
    /// frame count of last state change
    int StateStartFrame = 0;
};

} // namespace Oryol

 