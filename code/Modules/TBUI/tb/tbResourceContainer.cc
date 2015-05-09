//------------------------------------------------------------------------------
//  tbResourceContainer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "tbResourceContainer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
tbResourceContainer::tbResourceContainer() :
uniqueCounter(0) {
    // empty
}

//------------------------------------------------------------------------------
void
tbResourceContainer::setup(const TBUISetup& setup) {
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
tbResourceContainer::discard() {
    o_assert_dbg(this->isValid());

    this->resPool.Clear();
    this->freeSlots.Clear();

    resourceContainerBase::discard();
}

//------------------------------------------------------------------------------
void
tbResourceContainer::add(const Ptr<Stream>& data) {
    o_assert_dbg(this->isValid());
    Locator loc(data->GetURL().Get());
    Id id = this->registry.Lookup(loc);
    if (id.IsValid()) {
        Log::Warn("TBUIResourceContainer::Add(): resource '%s' already exists!\n", loc.Location().AsCStr());
    }
    else {
        Id newId(this->uniqueCounter++, this->freeSlots.Dequeue(), 0);
        this->registry.Add(loc, newId, this->peekLabel());
        this->resPool[newId.SlotIndex] = data;
    }
}

//------------------------------------------------------------------------------
void
tbResourceContainer::remove(ResourceLabel label) {
    o_assert_dbg(this->isValid());

    Array<Id> ids = this->registry.Remove(label);
    for (const Id& id : ids) {
        this->resPool[id.SlotIndex] = nullptr;
        this->freeSlots.Enqueue(id.SlotIndex);
    }
}

//------------------------------------------------------------------------------
Ptr<Stream>
tbResourceContainer::lookupResource(const Locator& loc) {
    o_assert_dbg(this->isValid());
    
    Id id = this->registry.Lookup(loc);
    if (id.IsValid()) {
        return resPool[id.SlotIndex];
    }
    else {
        return Ptr<Stream>();
    }
}

} // namespace _priv
} // namespace Oryol
