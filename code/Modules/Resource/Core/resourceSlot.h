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
    RESOURCE& Assign(const Id& id, const SETUP& setup, ResourceState::Code state, int32 frameCount);
    /// update the resource state
    void UpdateState(ResourceState::Code state, int32 frameCount);
    /// unassign the resource (sets state to ResourceState::Initial)
    void Unassign();
    
    /// direct access to resource
    RESOURCE Resource;
    /// resource state
    ResourceState::Code State = ResourceState::Initial;
    /// start frame count of current state
    int32 StateStartFrame = 0;
};

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> RESOURCE&
resourceSlot<RESOURCE,SETUP>::Assign(const Id& id, const SETUP& setup, ResourceState::Code state, int32 frameCount) {
    o_assert_dbg(ResourceState::Valid != this->State);
    this->State = state;
    this->StateStartFrame = frameCount;
    this->Resource.Id = id;
    this->Resource.Setup = setup;
    return this->Resource;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> void
resourceSlot<RESOURCE, SETUP>::UpdateState(ResourceState::Code state, int32 frameCount) {
    o_assert_dbg(ResourceState::Initial != this->State);
    this->State = state;
    this->StateStartFrame = frameCount;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> void
resourceSlot<RESOURCE,SETUP>::Unassign() {
    o_assert_dbg(ResourceState::Initial != this->State);
    this->Resource.Id.Invalidate();
    this->State = ResourceState::Initial;
    this->StateStartFrame = 0;
}

} // namespace Oryol
