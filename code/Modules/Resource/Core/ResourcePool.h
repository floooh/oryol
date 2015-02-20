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
#include "IO/Stream/Stream.h"

namespace Oryol {
    
template<class RESOURCE, class SETUP, class FACTORY> class ResourcePool {
public:
    /// max number of resources in a pool
    static const uint32 MaxNumPoolResources = (1<<16);

    /// constructor
    ResourcePool();
    /// destructor
    ~ResourcePool();
    
    /// setup the resource pool
    void Setup(FACTORY* factory, int32 poolSize, int32 maxCreatePerFrame, uint32 genericPlaceholderTypeFourcc);
    /// discard the resource pool
    void Discard();
    /// return true if the pool has been setup
    bool IsValid() const;
    /// update the pool, call once per frame
    void Update();
    
    /// allocate a resource id
    Id AllocId(uint8 resourceLabel);
    
    /// assign a resource to a free slot
    void Assign(const Id& id, const SETUP& setup);
    /// assign a resource to a free slot,
    void Assign(const Id& id, const SETUP& setup, const Ptr<Stream>& data);
    /// unassign/free a resource slot
    void Unassign(const Id& id);
    /// unassign all slots matching label by label (iterates over entire pool)
    void UnassignByLabel(uint8 label);
    /// return pointer to resource object, may return placeholder
    RESOURCE* Lookup(const Id& id) const;
    /// query the loading state of a contained resource
    ResourceState::Code QueryState(const Id& id) const;
    
    /// add a placeholder
    void RegisterPlaceholder(uint32 typeFourcc, const Id& id);
    
    /// get number of slots in pool
    int32 GetNumSlots() const;
    /// get number of used slots
    int32 GetNumUsedSlots() const;
    /// get number of free slots
    int32 GetNumFreeSlots() const;
    /// get number of pending slots
    int32 GetNumPendingSlots() const;
    
protected:
    /// free a resource id
    void freeId(const Id& id);
    /// lookup placeholder
    RESOURCE* lookupPlaceholder(uint32 typeFourcc) const;

    bool isValid;
    FACTORY* factory;
    int32 uniqueCounter;
    int32 maxNumCreatePerFrame;
    uint32 genericPlaceholderType;
    uint16 resourceType;
    
    Array<resourceSlot<RESOURCE,SETUP,FACTORY>> slots;
    Map<uint32, Id> placeholders;
    Queue<uint16> freeSlots;
    Array<uint16> pendingSlots;
};
    
//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY>
ResourcePool<RESOURCE,SETUP,FACTORY>::ResourcePool() :
isValid(false),
factory(nullptr),
uniqueCounter(0),
maxNumCreatePerFrame(0),
genericPlaceholderType(0),
resourceType(0xFFFF) {
    // empty
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY>
ResourcePool<RESOURCE,SETUP,FACTORY>::~ResourcePool() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
ResourcePool<RESOURCE,SETUP,FACTORY>::Setup(FACTORY* factory_, int32 poolSize, int32 maxCreatePerFrame, uint32 genericPlaceholderTypeFourcc) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(factory_);
    o_assert_dbg(poolSize > 0);
    
    this->factory = factory_;
    this->resourceType = this->factory->GetResourceType();
    this->maxNumCreatePerFrame = maxCreatePerFrame;
    this->genericPlaceholderType = genericPlaceholderTypeFourcc;
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
template<class RESOURCE, class SETUP, class FACTORY> void
ResourcePool<RESOURCE,SETUP,FACTORY>::Discard() {
    o_assert_dbg(this->isValid);
    // make sure that all resources had been freed (or should we do this here?)
    o_assert_dbg(this->freeSlots.Size() == this->slots.Size());
    this->isValid = false;
    
    this->slots.Clear();
    this->freeSlots.Clear();
    this->pendingSlots.Clear();
    this->placeholders.Clear();
    this->factory = nullptr;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> bool
ResourcePool<RESOURCE,SETUP,FACTORY>::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> Id
ResourcePool<RESOURCE,SETUP,FACTORY>::AllocId(uint8 resourceLabel) {
    o_assert_dbg(this->isValid);
    return Id(this->uniqueCounter++, this->freeSlots.Dequeue(), this->resourceType, resourceLabel);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
ResourcePool<RESOURCE,SETUP,FACTORY>::freeId(const Id& id) {
    o_assert_dbg(this->isValid);
    
    const uint16 slotIndex = id.SlotIndex();
    o_assert(this->slots[slotIndex].IsUnassigned());
    this->freeSlots.Enqueue(slotIndex);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
ResourcePool<RESOURCE,SETUP,FACTORY>::Assign(const Id& id, const SETUP& setup) {
    o_assert_dbg(this->isValid);
    
    const uint16 slotIndex = id.SlotIndex();
    auto& slot = this->slots[slotIndex];
    slot.Assign(this->factory, id, setup);
    if (slot.IsPending()) {
        // resource has started to load asynchronously
        this->pendingSlots.Add(slotIndex);
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
ResourcePool<RESOURCE,SETUP,FACTORY>::Assign(const Id& id, const SETUP& setup, const Ptr<Stream>& data) {
    o_assert_dbg(this->isValid);
    
    const uint16 slotIndex = id.SlotIndex();
    auto& slot = this->slots[slotIndex];
    slot.Assign(this->factory, id, setup, data);
    if (slot.IsPending()) {
        // resource has started to load asynchronously
        this->pendingSlots.Add(slotIndex);
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
ResourcePool<RESOURCE,SETUP,FACTORY>::Unassign(const Id& id) {
    o_assert_dbg(this->isValid);
    
    auto& slot = this->slots[id.SlotIndex()];
    if (slot.GetId() == id) {
        slot.Unassign(this->factory);
        this->freeId(id);
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
ResourcePool<RESOURCE,SETUP,FACTORY>::UnassignByLabel(uint8 label) {
    o_assert_dbg(this->isValid);
    
    for (auto& slot : this->slots) {
        Id id = slot.GetId();
        if (id.Label() == label) {
            slot.Unassign(this->factory);
            this->freeId(id);
        }        
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> RESOURCE*
ResourcePool<RESOURCE,SETUP,FACTORY>::Lookup(const Id& id) const {
    o_assert_dbg(this->isValid);
    o_assert_dbg(id.Type() == this->resourceType);
    
    const uint16 slotIndex = id.SlotIndex();
    auto& slot = this->slots[slotIndex];
    if (slot.GetId() == id) {
        if (slot.IsValid()) {
            // resource exists and is valid, all ok
            return (RESOURCE*) &slot.GetResource();
        }
        else {
            // resource exists but is not currently valid (pending or failed to load)
            // try to return matching placeholder
            o_error("FIXME FIXME FIXME");
            return nullptr;
        }
    }
    else {
        // resource does not currently exist, return generic placeholder
        return this->lookupPlaceholder(this->genericPlaceholderType);
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> ResourceState::Code
ResourcePool<RESOURCE,SETUP,FACTORY>::QueryState(const Id& id) const {
    o_assert_dbg(this->isValid);
    o_assert_dbg(id.Type() == this->resourceType);
    
    const uint16 slotIndex = id.SlotIndex();
    auto& slot = this->slots[slotIndex];
    if (slot.GetId() == id) {
        return slot.GetResource().GetState();
    }
    else {
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
ResourcePool<RESOURCE,SETUP,FACTORY>::RegisterPlaceholder(uint32 typeFourcc, const Id& id) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!this->placeholders.Contains(typeFourcc));
    this->placeholders.Add(typeFourcc, id);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> RESOURCE*
ResourcePool<RESOURCE,SETUP,FACTORY>::lookupPlaceholder(uint32 typeFourcc) const {
    o_assert_dbg(this->isValid);
    
    int32 index = this->placeholders.FindIndex(typeFourcc);
    if (InvalidIndex != index) {
        // can't simply call Lookup() here, or we'd risk an infinite recursion
        // if the placeholder resource isn't valid
        const Id& id = this->placeholders.ValueAtIndex(index);
        auto& slot = this->slots[id.SlotIndex()];
        if ((slot.GetId() == id) && slot.IsValid()) {
            return (RESOURCE*) &slot.GetResource();
        }
    }
    
    // fallthrough: placeholder not found, or not currently valid
    return nullptr;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
ResourcePool<RESOURCE,SETUP,FACTORY>::Update() {
    o_assert_dbg(this->isValid);
    
    // go over pending slots (these are currently loading), and call their update
    // method, break if maxNumCreatePerFrame is reached
    int32 numCreated = 0;
    for (int32 i = this->pendingSlots.Size() - 1; i >= 0; --i) {
        uint16 slotIndex = this->pendingSlots[i];
        auto& slot = this->slots[slotIndex];
        if (slot.ReadyForValidate(this->factory)) {
            // ok, slot is done loading, call the validate method and remove from pending array
            slot.Validate(this->factory);
            this->pendingSlots.Erase(i);
            
            // perform throttling if enabled
            numCreated++;
            if ((this->maxNumCreatePerFrame > 0) && (numCreated >= this->maxNumCreatePerFrame)) {
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> int32
ResourcePool<RESOURCE,SETUP,FACTORY>::GetNumSlots() const {
    return this->slots.Size();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> int32
ResourcePool<RESOURCE,SETUP,FACTORY>::GetNumUsedSlots() const {
    return this->slots.Size() - this->freeSlots.Size() - this->pendingSlots.Size();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> int32
ResourcePool<RESOURCE,SETUP,FACTORY>::GetNumFreeSlots() const {
    return this->freeSlots.Size();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> int32
ResourcePool<RESOURCE,SETUP,FACTORY>::GetNumPendingSlots() const {
    return this->pendingSlots.Size();
}

} // namespace Oryol
