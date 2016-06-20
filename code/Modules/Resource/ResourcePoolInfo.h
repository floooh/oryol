#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourcePoolInfo
    @ingroup Resource
    @brief detailed resource pool information

    Note the querying resource pool information is relatively slow
    since the function must iterate over all slots.
*/
#include "Core/Containers/StaticArray.h"
#include "Resource/ResourceState.h"

namespace Oryol {

class ResourcePoolInfo {
public:
    /// constructor
    ResourcePoolInfo() {
        this->NumSlotsByState.Fill(0);
    }
    
    /// number of resource slots by their state
    StaticArray<int, ResourceState::NumStates> NumSlotsByState;
    /// resource type of the pool
    uint16_t ResourceType = Id::InvalidType;
    /// overall number of slots
    int NumSlots = 0;
    /// number of used slots
    int NumUsedSlots = 0;
    /// number of free slots
    int NumFreeSlots = 0;
};

} // namespace Oryol
