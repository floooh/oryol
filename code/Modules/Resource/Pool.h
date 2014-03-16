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
#include "Resource/Id.h"
#include "Resource/Slot.h"
#include "IO/Stream.h"

namespace Oryol {
namespace Resource {
    
template<class RESOURCE, class FACTORY> class Pool {
public:
    /// constructor
    Pool();
    /// destructor
    ~Pool();
    
    /// setup the resource pool
    void Setup(FACTORY* factory, int32 poolSize, int32 maxCreatePerFrame);
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
    void Assign(const Id& id, const SETUP& setup);
    /// assign a resource to a free slot,
    void Assign(const Id& id, const SETUP& setup, const Ptr<IO::Stream>& data);
    /// unassign/free a resource slot
    void Unassign(const Id& id);
    /// return pointer to resource object, may return placeholder
    RESOURCE* Lookup(const Id& id) const;
    
    /// add a placeholder
    void RegisterPlaceholder(uint32 resTypeFourcc, const Id& id);
    /// lookup placeholder
    RESOURCE* LookupPlaceholder(uint32 resTypeFourcc) const;
    
    /// get number of slots in pool
    int32 GetNumSlots() const;
    /// get number of used slots
    int32 GetNumUsedSlots() const;
    /// get number of free slots
    int32 GetNumFreeSlots() const;
    /// get number of pending slots
    int32 GetNumPendingSlots() const;
    /// get id at slot index
    const Id& GetIdAtSlot(int32 slotIndex) const;
    /// get resource object at slot index
    const RESOURCE& GetResourceAtSlot(int32 slotIndex) const;
    
protected:
    bool isValid;
    FACTORY* factory;
    int32 uniqueCounter;
    int32 maxNumCreatePerFrame;
    
    Core::Array<ResourceSlot<RESOURCE,FACTORY>> slots;
    Core::Dictionary<uint32, Id> placeholders;
    Core::Queue<uint16> freeSlots;
    Core::Array<uint16> pendingSlots;
};
    
} // namespace Resource
} // namespace Oryol
