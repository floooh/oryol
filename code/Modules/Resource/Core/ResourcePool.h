#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourcePool
    @ingroup Resource
    @brief generic resource pool
    @todo ResourcePool description
*/
#include "Core/Ptr.h"
#include "Core/Containers/Queue.h"
#include "Core/Containers/Array.h"
#include "Resource/Id.h"
#include "Resource/ResourceInfo.h"
#include "Resource/ResourcePoolInfo.h"

namespace Oryol {
    
template<class RESOURCE, class SETUP> class ResourcePool {
public:
    /// max number of resources in a pool
    static const uint32 MaxNumPoolResources = (1<<16);

    /// constructor
    ResourcePool();
    /// destructor
    ~ResourcePool();
    
    /// setup the resource pool
    void Setup(Id::TypeT resourceType, int32 poolSize);
    /// discard the resource pool
    void Discard();
    /// return true if the pool has been setup
    bool IsValid() const;
    /// update the pool, call once per frame
    void Update();
    
    /// allocate a resource id
    Id AllocId();
    
    /// assign a resource to a free slot
    RESOURCE& Assign(const Id& id, const SETUP& setup, ResourceState::Code state);
    /// unassign/free a resource slot
    void Unassign(const Id& id);
    /// return pointer to resource object, may return placeholder or nullptr
    RESOURCE* Lookup(const Id& id) const;
    /// update the resource state of a contained resource
    void UpdateState(const Id& id, ResourceState::Code newState);
    /// test if the pool contains a slot with resource id (regardless of state)
    bool Contains(const Id& id) const;
    /// query the loading state of a contained resource
    ResourceState::Code QueryState(const Id& id) const;
    /// query additional info about a contained resource
    ResourceInfo QueryResourceInfo(const Id& id) const;
    /// query additional info about the pool (slow)
    ResourcePoolInfo QueryPoolInfo() const;
    
    /// get number of slots in pool
    int32 GetNumSlots() const;
    /// get number of used slots
    int32 GetNumUsedSlots() const;
    /// get number of free slots
    int32 GetNumFreeSlots() const;
    
protected:
    /// free a resource id
    void freeId(const Id& id);
    
    bool isValid;
    int32 frameCounter;
    int32 uniqueCounter;
    Id::TypeT resourceType;
    
    Array<RESOURCE> slots;
    Queue<uint16> freeSlots;
};
    
//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP>
ResourcePool<RESOURCE,SETUP>::ResourcePool() :
isValid(false),
frameCounter(0),
uniqueCounter(0),
resourceType(0xFF) {
    // empty
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP>
ResourcePool<RESOURCE,SETUP>::~ResourcePool() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> void
ResourcePool<RESOURCE,SETUP>::Setup(Id::TypeT resType, int32 poolSize) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(Id::InvalidType != resType);
    o_assert_dbg(poolSize > 0);
    
    this->resourceType = resType;
    this->slots.Reserve(poolSize);
    this->slots.SetAllocStrategy(0, 0);    // disable growing
    this->freeSlots.Reserve(poolSize);
    
    // setup empty slots
    for (int32 i = 0; i < poolSize; i++) {
        this->slots.Add();
    }
    
    // setup free slots queue
    for (uint16 i = 0; i < poolSize; i++) {
        this->freeSlots.Enqueue(i);
    }
    
    this->isValid = true;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> void
ResourcePool<RESOURCE,SETUP>::Discard() {
    o_assert_dbg(this->isValid);
    // make sure that all resources had been freed (or should we do this here?)
    o_assert_dbg(this->freeSlots.Size() == this->slots.Size());
    this->isValid = false;
    
    this->slots.Clear();
    this->freeSlots.Clear();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> bool
ResourcePool<RESOURCE,SETUP>::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> void
ResourcePool<RESOURCE, SETUP>::Update() {
    o_assert_dbg(this->isValid);
    this->frameCounter++;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> Id
ResourcePool<RESOURCE,SETUP>::AllocId() {
    o_assert_dbg(this->isValid);
    o_assert_dbg(Id::InvalidType != this->resourceType);
    Id newId(this->uniqueCounter++, this->freeSlots.Dequeue(), this->resourceType);
    #if ORYOL_DEBUG
        const auto& slot = this->slots[newId.SlotIndex];
        o_assert_dbg(ResourceState::Initial == slot.State);
    #endif
    return newId;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> void
ResourcePool<RESOURCE,SETUP>::freeId(const Id& id) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(ResourceState::Initial == this->slots[id.SlotIndex].State);
    this->freeSlots.Enqueue(id.SlotIndex);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> RESOURCE&
ResourcePool<RESOURCE,SETUP>::Assign(const Id& id, const SETUP& setup, ResourceState::Code state) {
    o_assert_dbg(this->isValid);
    
    auto& slot = this->slots[id.SlotIndex];
    o_assert_dbg(ResourceState::Valid != slot.State);
    slot.State = state;
    slot.StateStartFrame = this->frameCounter;
    slot.Id = id;
    slot.Setup = setup;
    return slot;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> void
ResourcePool<RESOURCE,SETUP>::Unassign(const Id& id) {
    o_assert_dbg(this->isValid);
    
    auto& slot = this->slots[id.SlotIndex];
    if (id == slot.Id) {
        o_assert_dbg(ResourceState::Initial != slot.State);
        slot.Id.Invalidate();
        slot.State = ResourceState::Initial;
        slot.StateStartFrame = 0;
        this->freeId(id);
    }
    else {
        o_warn("ResourcePool::Unassign(): id not in pool (type: '%d', slot: '%d')\n", id.Type, id.SlotIndex);
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> RESOURCE*
ResourcePool<RESOURCE,SETUP>::Lookup(const Id& id) const {
    o_assert_dbg(this->isValid);
    o_assert_dbg(id.Type == this->resourceType);
    
    const auto& slot = this->slots[id.SlotIndex];
    if (id == slot.Id) {
        if (ResourceState::Valid == slot.State) {
            // resource exists and is valid or pending, all ok
            return const_cast<RESOURCE*>(&slot);
        }
        // FIXME: return placeholder if one is defined
    }
    return nullptr;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> void
ResourcePool<RESOURCE, SETUP>::UpdateState(const Id& id, ResourceState::Code newState) {
    o_assert_dbg(this->isValid);
    auto& slot = this->slots[id.SlotIndex];
    if (id == slot.Id) {
        o_assert_dbg(ResourceState::Initial != slot.State);
        slot.State = newState;
        slot.StateStartFrame = this->frameCounter;
    }
    else {
        o_warn("ResourcePool::UpdateState(): id not in pool (type: '%d', slot: '%d')\n", id.Type, id.SlotIndex);
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> bool
ResourcePool<RESOURCE, SETUP>::Contains(const Id& id) const {
    o_assert_dbg(this->isValid);
    o_assert_dbg(id.Type == this->resourceType);
    return id == this->slots[id.SlotIndex].Id;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> ResourceState::Code
ResourcePool<RESOURCE,SETUP>::QueryState(const Id& id) const {
    o_assert_dbg(this->isValid);
    o_assert_dbg(id.Type == this->resourceType);
    
    const auto& slot = this->slots[id.SlotIndex];
    if (id == slot.Id) {
        return slot.State;
    }
    else {
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> ResourceInfo
ResourcePool<RESOURCE, SETUP>::QueryResourceInfo(const Id& id) const {
    o_assert_dbg(this->isValid);
    o_assert_dbg(id.Type == this->resourceType);
    
    ResourceInfo info;
    const auto& slot = this->slots[id.SlotIndex];
    if (id == slot.Id) {
        info.State = slot.State;
        info.StateAge = this->frameCounter - slot.StateStartFrame;
    }
    return info;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> ResourcePoolInfo
ResourcePool<RESOURCE, SETUP>::QueryPoolInfo() const {
    o_assert_dbg(this->isValid);
    
    ResourcePoolInfo poolInfo;
    poolInfo.ResourceType = this->resourceType;
    poolInfo.NumSlots = this->GetNumSlots();
    poolInfo.NumUsedSlots = this->GetNumUsedSlots();
    poolInfo.NumFreeSlots = this->GetNumFreeSlots();
    for (const auto& slot : this->slots) {
        if (ResourceState::InvalidState != slot.State) {
            poolInfo.NumSlotsByState[slot.State]++;
        }
    }
    return poolInfo;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> int32
ResourcePool<RESOURCE,SETUP>::GetNumSlots() const {
    return this->slots.Size();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> int32
ResourcePool<RESOURCE,SETUP>::GetNumUsedSlots() const {
    return this->slots.Size() - this->freeSlots.Size();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> int32
ResourcePool<RESOURCE,SETUP>::GetNumFreeSlots() const {
    return this->freeSlots.Size();
}

} // namespace Oryol
