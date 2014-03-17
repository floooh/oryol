#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Resource::Pool
    @brief generic resource pool
    @todo Resource::Pool description
*/
#include "Core/Ptr.h"
#include "Core/Containers/Queue.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Map.h"
#include "Resource/Id.h"
#include "Resource/Slot.h"
#include "IO/Stream.h"

namespace Oryol {
namespace Resource {
    
template<class RESOURCE, class FACTORY> class Pool {
public:
    /// max number of resources in a pool
    static const uint32 = MaxNumPoolResources = (1<<16);

    /// constructor
    Pool();
    /// destructor
    ~Pool();
    
    /// setup the resource pool
    void Setup(FACTORY* factory, int32 poolSize, int32 maxCreatePerFrame, uint32 genericPlaceholderTypeFourcc);
    /// discard the resource pool
    void Discard();
    /// return true if the pool has been setup
    bool IsValid() const;
    /// update the pool, call once per frame
    void Update();
    
    /// allocate a resource id
    Id AllocId();
    
    /// assign a resource to a free slot
    void Assign(const Id& id, const SETUP& setup);
    /// assign a resource to a free slot,
    void Assign(const Id& id, const SETUP& setup, const Ptr<IO::Stream>& data);
    /// unassign/free a resource slot
    void Unassign(const Id& id);
    /// return pointer to resource object, may return placeholder
    const RESOURCE* Lookup(const Id& id) const;
    
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
    const RESOURCE* lookupPlaceholder(uint32 typeFourcc) const;

    bool isValid;
    FACTORY* factory;
    int32 uniqueCounter;
    int32 maxNumCreatePerFrame;
    uin32 genericPlaceholderType;
    uint16 resourceType;
    
    Core::Array<slot<RESOURCE,FACTORY>> slots;
    Core::Map<uint32, Id> placeholders;
    Core::Queue<uint16> freeSlots;
    Core::Array<uint16> pendingSlots;
};
    
//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY>
Pool<RESOURCE,FACTORY>::Pool() :
isValid(false),
factory(nullptr),
uniqueCounter(0),
maxNumCreatePerFrame(0),
genericPlaceholderType(0),
resourceType(0xFFFF) {
    // empty
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY>
Pool<RESOURCE,FACTORY>::~Pool() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> void
Pool<RESOURCE,FACTORY>::Setup(FACTORY* factory_, int32 poolSize, int32 maxCreatePerFrame, uint32 genericPlaceholderTypeFourcc) {
    o_assert(!this->isValid);
    o_assert(factory_)
    o_assert(poolSize > 0);
    
    this->maxNumCreatePerFrame = maxCreatePerFrame;
    this->genericPlaceholderType = genericPlaceholderTypeFourcc;
    this->slots.Reserve(poolSize);
    this->slots.AllocStrategy(0, 0);    // make this a fixed-size array
    this->freeSlots.Reserve(poolSize);
    
    // setup empty slots
    for (int32 i = 0; i < poolSize; i++) {
        this->slots.EmplaceBack();
    }
    
    // setup free slots queue
    for (uint16 i = 0; i < poolSize; i++) {
        this->freeSlots.Enqueue(i);
    }
    
    this->isValid = true;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> void
Pool<RESOURCE,FACTORY>::Discard() {
    o_assert(this->isValid);
    // make sure that all resources had been freed (or should we do this here?)
    o_assert(this->freeSlots.Size() == this->slots.Size());
    
    this->slots.Clear();
    this->freeSlots.Clear();
    this->pendingSlots.Clear();
    this->placeholders.Clear();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> void
Pool<RESOURCE,FACTORY>::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> Id
Pool<RESOURCE,FACTORY>::AllocId() {
    o_assert(this->isValid);
    return Id(this->uniqueCounter++, this->freeSlots.Dequeue(), this->resourceType);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> void
Pool<RESOURCE,FACTORY>::FreeId(const Id& id) {
    o_assert(this->isValid);
    
    const uint16 slotIndex = id.SlotIndex();
    o_assert(this->slots[slotIndex].IsUnassigned());
    this->freeSlots.Enqueue(slotIndex);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> void
Pool<RESOURCE,FACTORY>::Assign(const Id& id, const SETUP& setup) {
    o_assert(this->isValid);
    
    const uint16 slotIndex = id.SlotIndex();
    auto& slot = this->slots[slotIndex];
    slot.Assign(this->factory, id, setup);
    if (slot.IsPending()) {
        // resource has started to load asynchronously
        this->pendingSlots.AddBack(slotIndex);
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> void
Pool<RESOURCE,FACTORY>::Assign(const Id& id, const SETUP& setup, const Ptr<IO::Stream>& data) {
    o_assert(this->isValid);
    
    const uint16 slotIndex = id.SlotIndex();
    auto& slot = this->slots[slotIndex];
    slot.Assign(this->factory, id, setup, data);
    if (slot.IsPending()) {
        // resource has started to load asynchronously
        this->pendingSlots.AddBack(slotIndex);
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> void
Pool<RESOURCE,FACTORY>::Unassign(const Id& id) {
    o_assert(this->isValid);
    
    auto& slot = this->slots[id.SlotIndex()];
    if (slot.GetId() == id) {
        slot.Unassign(this->factory);
        this->freeId(id);
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> void
Pool<RESOURCE,FACTORY>::Lookup(const Id& id) const {
    o_assert(this->isValid);
    
    const uint16 slotIndex = id.SlotIndex();
    const auto& slot = this->slots[slotIndex];
    if (slot.GetId() == id) {
        if (slot.IsValid()) {
            // resource exists and is valid, all ok
            return &slot.GetResource();
        }
        else {
            // resource exists but is not currently valid (pending or failed to load)
            // try to return matching placeholder
            return this->LookupPlaceholder(slot.GetPlaceholderType());
        }
    }
    else {
        // resource does not currently exist, return generic placeholder
        return this->LookupPlaceholder(this->genericPlaceholderType);
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> void
Pool<RESOURCE,FACTORY>::RegisterPlaceholder(uint32 typeFourcc, const Id& id) {
    o_assert(this->isValid);
    o_assert(!this->placeholders.Contains(typeFourcc));
    this->placeholders.Insert(typeFourcc, id);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> const RESOURCE*
Pool<RESOURCE,FACTORY>::LookupPlaceholder(uint32 typeFourcc) const {
    o_assert(this->isValid);
    
    int32 index = this->placeholders.FindIndex(typeFourcc);
    if (InvalidIndex != index) {
        // can't simply call Lookup() here, or we'd risk an infinite recursion
        // if the placeholder resource isn't valid
        const Id& id = this->placeholders.ValueAtIndex(index);
        const auto& slot = this->slots[id.SlotIndex()];
        if ((slot.GetId() == id) && slot.IsValid()) {
            return &slot.GetResource();
        }
    }
    
    // fallthrough: placeholder not found, or not currently valid
    return nullptr;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> void
Pool<RESOURCE,FACTORY>::Update() {
    o_assert(this->isValid);
    
    // go over pending slots (these are currently loading), and call their update
    // method, break if maxNumCreatePerFrame is reached
    int32 numCreated = 0;
    for (int32 i = this->pendingSlots.Size() - 1; i >= 0; --i) {
        uint16 slotIndex = this->pendingSlots[i];
        auto& slot = this->slots[slotIndex];
        if (slot.ReadyForValidate(this->factory)) {
            // ok, slot is done loading, call the validate method and remove from pending array
            slot.Validate(this->factory);
            this->pendingSlots.EraseIndex(i);
            
            // perform throttling if enabled
            numCreated++;
            if ((this->maxNumCreatePerFrame > 0) && (numCreated >= this->maxNumCreatePerFrame)) {
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> void
Pool<RESOURCE,FACTORY>::GetNumSlots() const {
    return this->slots.Size();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> int32
Pool<RESOURCE,FACTORY>::GetNumUsedSlots() const {
    return this->slots.Size() - this->freeSlots.Size() - this->pendingSlots.Size();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> int32
Pool<RESOURCE,FACTORY>::GetNumFreeSlots() const {
    return this->freeSlots.Size();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class FACTORY> int32
Pool<RESOURCE,FACTORY>::GetNumPendingSlots() const {
    return this->pendingSlots.Size();
}

} // namespace Resource
} // namespace Oryol
