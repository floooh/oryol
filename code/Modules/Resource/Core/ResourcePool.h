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
#include "Core/Containers/Map.h"
#include "Resource/Id.h"
#include "Resource/Core/resourceSlot.h"

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
    void Setup(uint8 resourceType, int32 poolSize);
    /// discard the resource pool
    void Discard();
    /// return true if the pool has been setup
    bool IsValid() const;
    /// update the pool, call once per frame
    void Update();
    
    /// allocate a resource id
    Id AllocId(uint8 resourceLabel);
    
    /// assign a resource to a free slot
    RESOURCE& Assign(const Id& id, const SETUP& setup, ResourceState::Code state);
    /// unassign/free a resource slot
    void Unassign(const Id& id);
    /// unassign all slots matching label by label (iterates over entire pool)
    void UnassignByLabel(uint8 label);
    /// return pointer to resource object, may return placeholder or nullptr
    RESOURCE* Lookup(const Id& id) const;
    /// lookup 'raw' resource, may return nullptr
    RESOURCE* Get(const Id& id) const;
    /// query the loading state of a contained resource
    ResourceState::Code QueryState(const Id& id) const;
    
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
    int32 uniqueCounter;
    uint8 resourceType;
    
    Array<resourceSlot<RESOURCE,SETUP>> slots;
    Queue<uint16> freeSlots;
};
    
//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP>
ResourcePool<RESOURCE,SETUP>::ResourcePool() :
isValid(false),
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
ResourcePool<RESOURCE,SETUP>::Setup(uint8 resType, int32 poolSize) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(Id::InvalidType != resType);
    o_assert_dbg(poolSize > 0);
    
    this->resourceType = resType;
    this->slots.Reserve(poolSize);
    this->slots.SetAllocStrategy(0, 0);    // make this a fixed-size array
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
template<class RESOURCE, class SETUP> Id
ResourcePool<RESOURCE,SETUP>::AllocId(uint8 resourceLabel) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(Id::InvalidType != this->resourceType);
    return Id(this->uniqueCounter++, this->freeSlots.Dequeue(), this->resourceType, resourceLabel);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> void
ResourcePool<RESOURCE,SETUP>::freeId(const Id& id) {
    o_assert_dbg(this->isValid);
    
    const uint16 slotIndex = id.SlotIndex();
    o_assert(ResourceState::Initial == this->slots[slotIndex].Resource.State);
    this->freeSlots.Enqueue(slotIndex);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> RESOURCE&
ResourcePool<RESOURCE,SETUP>::Assign(const Id& id, const SETUP& setup, ResourceState::Code state) {
    o_assert_dbg(this->isValid);
    
    const uint16 slotIndex = id.SlotIndex();
    auto& slot = this->slots[slotIndex];
    slot.Assign(id, setup, state);
    return slot.Resource;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> void
ResourcePool<RESOURCE,SETUP>::Unassign(const Id& id) {
    o_assert_dbg(this->isValid);
    
    auto& slot = this->slots[id.SlotIndex()];
    if (id == slot.Resource.Id) {
        slot.Unassign();
        this->freeId(id);
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> void
ResourcePool<RESOURCE,SETUP>::UnassignByLabel(uint8 label) {
    o_assert_dbg(this->isValid);
    
    for (auto& slot : this->slots) {
        const Id id = slot.Resource.Id;
        if (id.Label() == label) {
            slot.Unassign();
            this->freeId(id);
        }        
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> RESOURCE*
ResourcePool<RESOURCE,SETUP>::Lookup(const Id& id) const {
    o_assert_dbg(this->isValid);
    o_assert_dbg(id.Type() == this->resourceType);
    
    const uint16 slotIndex = id.SlotIndex();
    auto& slot = this->slots[slotIndex];
    if (id == slot.Resource.Id) {
        if (ResourceState::Valid == slot.Resource.State) {
            // resource exists and is valid, all ok
            return (RESOURCE*) &slot.Resource;
        }
    }
    // FALLTHROUGH: no valid resource (doesn't exist, is pending, failed etc...)
    o_error("FIXME FIXME FIXME");
    return nullptr;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> RESOURCE*
ResourcePool<RESOURCE,SETUP>::Get(const Id& id) const {
    o_assert_dbg(this->isValid);
    o_assert_dbg(id.Type() == this->resourceType);
    
    const uint16 slotIndex = id.SlotIndex();
    auto& slot = this->slots[slotIndex];
    if (id == slot.Resource.Id) {
        return (RESOURCE*) &slot.Resource;
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> ResourceState::Code
ResourcePool<RESOURCE,SETUP>::QueryState(const Id& id) const {
    o_assert_dbg(this->isValid);
    o_assert_dbg(id.Type() == this->resourceType);
    
    const uint16 slotIndex = id.SlotIndex();
    auto& slot = this->slots[slotIndex];
    if (id == slot.Resource.Id) {
        return slot.Resource.State;
    }
    else {
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> int32
ResourcePool<RESOURCE,SETUP>::GetNumSlots() const {
    return this->slots.Size();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> int32
ResourcePool<RESOURCE,SETUP>::GetNumUsedSlots() const {
    return this->slots.Size() - this->freeSlots.Size() - this->pendingSlots.Size();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP> int32
ResourcePool<RESOURCE,SETUP>::GetNumFreeSlots() const {
    return this->freeSlots.Size();
}

} // namespace Oryol
