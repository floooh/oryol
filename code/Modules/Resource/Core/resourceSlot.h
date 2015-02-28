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
    RESOURCE& Assign(const Id& id, const SETUP& setup, ResourceState::Code state);
    /// unassign the resource
    void Unassign(ResourceState::Code state);
    
    /// direct access to resource
    RESOURCE Resource;
    /// resource state
    ResourceState::Code State = ResourceState::Initial;
};

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> RESOURCE&
resourceSlot<RESOURCE,SETUP>::Assign(const Id& id, const SETUP& setup, ResourceState::Code state) {
    o_assert_dbg(ResourceState::Initial == this->State);
    this->State = state;
    this->Resource.Id = id;
    this->Resource.Setup = setup;
    return this->Resource;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> void
resourceSlot<RESOURCE,SETUP>::Unassign(ResourceState::Code state) {
    o_assert_dbg(ResourceState::Initial != this->State);
    this->State = state;
}

} // namespace Oryol
