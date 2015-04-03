//------------------------------------------------------------------------------
//  TBUIResourceContainer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TBUIResourceContainer.h"

namespace Oryol {

using namespace _priv;

//------------------------------------------------------------------------------
TBUIResourceContainer::TBUIResourceContainer() :
uniqueCounter(0) {
    // empty
}

//------------------------------------------------------------------------------
void
TBUIResourceContainer::setup(const TBUISetup& setup) {
    o_assert_dbg(!this->isValid());
    
    this->resPool.Reserve(setup.ResourcePoolSize);
    this->resPool.SetAllocStrategy(0, 0);
    this->freeSlots.Reserve(setup.ResourcePoolSize);
    
    for (uint16 i = 0; i < setup.ResourcePoolSize; i++) {
        this->resPool.Add();
        this->freeSlots.Enqueue(i);
    }

    resourceContainerBase::setup(setup.ResourceLabelStackCapacity, setup.ResourceRegistryCapacity);
}

//------------------------------------------------------------------------------
void
TBUIResourceContainer::discard() {
    o_assert_dbg(this->isValid());

    this->resPool.Clear();
    this->freeSlots.Clear();

    resourceContainerBase::discard();
}

//------------------------------------------------------------------------------
void
TBUIResourceContainer::Add(const Ptr<Stream>& data) {
    o_assert_dbg(this->isValid());
    Locator loc(data->GetURL().Get());
    o_assert_dbg(!this->registry.Lookup(loc).IsValid());
    Id newId(this->uniqueCounter++, this->freeSlots.Dequeue(), 0);
    this->registry.Add(loc, newId, this->peekLabel());
    this->resPool[newId.SlotIndex] = data;
}

//------------------------------------------------------------------------------
void
TBUIResourceContainer::Remove(ResourceLabel label) {
    o_assert_dbg(this->isValid());

    Array<Id> ids = this->registry.Remove(label);
    for (const Id& id : ids) {
        this->resPool[id.SlotIndex] = nullptr;
        this->freeSlots.Enqueue(id.SlotIndex);
    }
}

//------------------------------------------------------------------------------
Ptr<Stream>
TBUIResourceContainer::lookupResource(const Locator& loc) {
    o_assert_dbg(this->isValid());
    
    Id id = this->registry.Lookup(loc);
    if (id.IsValid()) {
        return resPool[id.SlotIndex];
    }
    else {
        return Ptr<Stream>();
    }
}

} // namespace Oryol
