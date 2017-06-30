#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourcePool
    @ingroup Resource
    @brief generic resource pool
    @todo ResourcePool description
*/
#include "Core/Containers/Queue.h"
#include "Core/Containers/Array.h"
#include "Resource/Id.h"
#include "Resource/ResourceInfo.h"
#include "Resource/ResourcePoolInfo.h"

namespace Oryol {
    
template<class RESOURCE> class ResourcePool {
public:
    /// max number of resources in a pool
    static const int MaxNumPoolResources = (1<<16);

    /// destructor
    ~ResourcePool();
    
    /// setup the resource pool
    void Setup(Id::TypeT resourceType, int poolSize);
    /// discard the resource pool
    void Discard();
    /// return true if the pool has been setup
    bool IsValid() const;
    /// update the pool, call once per frame
    void Update();
    
    /// allocate a resource id
    Id AllocId();
    /// free a resource id
    void FreeId(const Id& id);
    
    /// assign a resource to a free slot
    RESOURCE& Assign(const Id& id, ResourceState::Code state);
    /// unassign/free a resource slot
    void Unassign(const Id& id);
    /// return pointer to resource object, may return placeholder or nullptr
    RESOURCE* Lookup(const Id& id) const;
    /// get pointer to resource by resource id, only return nullptr if resource is not contained
    RESOURCE* Get(const Id& id) const;
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
    int GetNumSlots() const;
    /// get number of used slots
    int GetNumUsedSlots() const;
    /// get number of free slots
    int GetNumFreeSlots() const;
    
    /// there will be no allocated slots beyond this (but there may be holes!)
    Id::SlotIndexT LastAllocSlot = 0;

    bool isValid = false;
    int frameCounter = 0;
    int uniqueCounter = 0;
    Id::TypeT resourceType = 0xFF;
    
    Array<RESOURCE> slots;
    Queue<uint16_t> freeSlots;
};
    
//------------------------------------------------------------------------------
template<class RESOURCE>
ResourcePool<RESOURCE>::~ResourcePool() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
template<class RESOURCE> void
ResourcePool<RESOURCE>::Setup(Id::TypeT resType, int poolSize) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(Id::InvalidType != resType);
    o_assert_dbg(poolSize > 0);
    
    this->resourceType = resType;
    this->slots.SetFixedCapacity(poolSize);
    this->freeSlots.SetFixedCapacity(poolSize);
    this->LastAllocSlot = 0;
    
    // setup empty slots
    for (int i = 0; i < poolSize; i++) {
        this->slots.Add();
    }
    
    // setup free slots queue
    for (uint16_t i = 0; i < poolSize; i++) {
        this->freeSlots.Enqueue(i);
    }
    
    this->isValid = true;
}

//------------------------------------------------------------------------------
template<class RESOURCE> void
ResourcePool<RESOURCE>::Discard() {
    o_assert_dbg(this->isValid);
    // make sure that all resources had been freed (or should we do this here?)
    o_assert_dbg(this->freeSlots.Size() == this->slots.Size());
    this->isValid = false;
    this->LastAllocSlot = 0;    
    this->slots.Clear();
    this->freeSlots.Clear();
}

//------------------------------------------------------------------------------
template<class RESOURCE> bool
ResourcePool<RESOURCE>::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
template<class RESOURCE> void
ResourcePool<RESOURCE>::Update() {
    o_assert_dbg(this->isValid);
    this->frameCounter++;
}

//------------------------------------------------------------------------------
template<class RESOURCE> Id
ResourcePool<RESOURCE>::AllocId() {
    o_assert_dbg(this->isValid);
    o_assert_dbg(Id::InvalidType != this->resourceType);
    Id newId(this->uniqueCounter++, this->freeSlots.Dequeue(), this->resourceType);
    #if ORYOL_DEBUG
        const auto& slot = this->slots[newId.SlotIndex];
        o_assert_dbg(ResourceState::Initial == slot.State);
    #endif
    if (newId.SlotIndex > this->LastAllocSlot) {
        this->LastAllocSlot = newId.SlotIndex;
    }
    return newId;
}

//------------------------------------------------------------------------------
template<class RESOURCE> void
ResourcePool<RESOURCE>::FreeId(const Id& id) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!this->slots[id.SlotIndex].Id.IsValid());
    o_assert_dbg(ResourceState::Initial == this->slots[id.SlotIndex].State);
    o_assert_dbg(id.SlotIndex <= this->LastAllocSlot);
    // find the next highest 'last alloc slot' 
    while ((this->LastAllocSlot > 0) && !this->slots[this->LastAllocSlot].Id.IsValid()) {
        this->LastAllocSlot--;
    }
    this->freeSlots.Enqueue(id.SlotIndex);
}

//------------------------------------------------------------------------------
template<class RESOURCE> RESOURCE&
ResourcePool<RESOURCE>::Assign(const Id& id, ResourceState::Code state) {
    o_assert_dbg(this->isValid);
    
    auto& slot = this->slots[id.SlotIndex];
    o_assert_dbg(ResourceState::Valid != slot.State);
    slot.State = state;
    slot.StateStartFrame = this->frameCounter;
    slot.Id = id;
    return slot;
}

//------------------------------------------------------------------------------
template<class RESOURCE> void
ResourcePool<RESOURCE>::Unassign(const Id& id) {
    o_assert_dbg(this->isValid);
    
    auto& slot = this->slots[id.SlotIndex];
    if (id == slot.Id) {
        o_assert_dbg(ResourceState::Initial != slot.State);
        slot.Id.Invalidate();
        slot.State = ResourceState::Initial;
        slot.StateStartFrame = 0;
        this->FreeId(id);
    }
    else {
        o_warn("ResourcePool::Unassign(): id not in pool (type: '%d', slot: '%d')\n", id.Type, id.SlotIndex);
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE> RESOURCE*
ResourcePool<RESOURCE>::Lookup(const Id& id) const {
    o_assert_dbg(this->isValid);
    if (!id.IsValid()) {
        return nullptr;
    }
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
template<class RESOURCE> RESOURCE*
ResourcePool<RESOURCE>::Get(const Id& id) const {
    o_assert_dbg(this->isValid);
    o_assert_dbg(id.Type == this->resourceType);
    const auto& slot = this->slots[id.SlotIndex];
    if (id == slot.Id) {
        return const_cast<RESOURCE*>(&slot);
    }
    else {
        // dangling Id, resource slot has been re-occupied
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE> void
ResourcePool<RESOURCE>::UpdateState(const Id& id, ResourceState::Code newState) {
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
template<class RESOURCE> bool
ResourcePool<RESOURCE>::Contains(const Id& id) const {
    o_assert_dbg(this->isValid);
    o_assert_dbg(id.Type == this->resourceType);
    return id == this->slots[id.SlotIndex].Id;
}

//------------------------------------------------------------------------------
template<class RESOURCE> ResourceState::Code
ResourcePool<RESOURCE>::QueryState(const Id& id) const {
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
template<class RESOURCE> ResourceInfo
ResourcePool<RESOURCE>::QueryResourceInfo(const Id& id) const {
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
template<class RESOURCE> ResourcePoolInfo
ResourcePool<RESOURCE>::QueryPoolInfo() const {
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
template<class RESOURCE> int
ResourcePool<RESOURCE>::GetNumSlots() const {
    return this->slots.Size();
}

//------------------------------------------------------------------------------
template<class RESOURCE> int
ResourcePool<RESOURCE>::GetNumUsedSlots() const {
    return this->slots.Size() - this->freeSlots.Size();
}

//------------------------------------------------------------------------------
template<class RESOURCE> int
ResourcePool<RESOURCE>::GetNumFreeSlots() const {
    return this->freeSlots.Size();
}

} // namespace Oryol
