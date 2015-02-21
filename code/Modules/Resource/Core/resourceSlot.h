#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::resourceSlot
    @ingroup Resource
    @brief a slot in a ResourcePool
*/
#include "Resource/Id.h"
#include "Resource/ResourceState.h"

namespace Oryol {
    
template<class RESOURCE, class SETUP> class resourceSlot {
public:
    /// assign a resource to the slot
    RESOURCE& Assign(const Id& id, const SETUP& setup);
    /// unassign the resource
    void Unassign();
    
    /// direct access to resource
    RESOURCE Resource;
};

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> RESOURCE&
resourceSlot<RESOURCE,SETUP>::Assign(const Id& id, const SETUP& setup) {
    o_assert(ResourceState::Initial == this->Resource.State);
    this->Resource.Id = id;
    this->Resource.Setup = setup;
    this->Resource.State = ResourceState::Setup;
    return this->Resource;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> void
resourceSlot<RESOURCE,SETUP>::Unassign() {
    o_assert(ResourceState::Initial != this->Resource.State);
    this->Resource.State = ResourceState::Initial;
}

} // namespace Oryol
