//------------------------------------------------------------------------------
//  Registry.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Registry.h"

namespace Oryol {
namespace Resource {

//------------------------------------------------------------------------------
Registry::Registry() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
Registry::~Registry() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void
Registry::Setup(int32 reserveSize) {
    o_assert(!this->isValid);
    
    this->isValid = true;
    this->entries.Reserve(reserveSize);
    this->locatorIndexMap.Reserve(reserveSize);
    this->idIndexMap.Reserve(reserveSize);
}

//------------------------------------------------------------------------------
void
Registry::Discard() {
    o_assert(this->isValid);
    
    this->entries.Clear();
    this->locatorIndexMap.Clear();
    this->idIndexMap.Clear();
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
Registry::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
Registry::AddResource(const Locator& loc, const Id& id) {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    o_assert(!this->idIndexMap.Contains(id));
    
    this->entries.Emplace(loc, id);
    if (loc.IsShared()) {
        o_assert(!this->locatorIndexMap.Contains(loc));
        this->locatorIndexMap.Add(loc, this->entries.Size() - 1);
    }
    this->idIndexMap.Add(id, this->entries.Size() - 1);
    
    this->incrUseCount(id);
}

//------------------------------------------------------------------------------
void
Registry::AddResource(const Locator& loc, const Id& id, const Array<Id>& deps) {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    o_assert(deps.Size() < MaxNumDependents);
    o_assert(!this->idIndexMap.Contains(id));
    
    this->entries.Emplace(loc, id);
    Entry& entry = this->entries.Back();
    for (const Id& depId : deps) {
        o_assert(depId.IsValid());
        o_assert(depId != id);
        entry.deps[entry.numDeps++] = depId;
    }
    if (loc.IsShared()) {
        o_assert(!this->locatorIndexMap.Contains(loc));
        this->locatorIndexMap.Add(loc, this->entries.Size() - 1);
    }
    this->idIndexMap.Add(id, this->entries.Size() - 1);
    
    this->incrUseCount(id);
}

//------------------------------------------------------------------------------
const Registry::Entry*
Registry::findEntryByLocator(const Locator& loc) const {
    if (loc.IsShared()) {
        const int32 mapIndex = this->locatorIndexMap.FindIndex(loc);
        if (InvalidIndex != mapIndex) {
            const int32 entryIndex = this->locatorIndexMap.ValueAtIndex(mapIndex);
            return &(this->entries[entryIndex]);
        }
    }
    return nullptr;
}

//------------------------------------------------------------------------------
const Registry::Entry*
Registry::findEntryById(const Id& id) const {
    const int32 mapIndex = this->idIndexMap.FindIndex(id);
    if (InvalidIndex != mapIndex) {
        const int32 entryIndex = this->idIndexMap.ValueAtIndex(mapIndex);
        return &(this->entries[entryIndex]);
    }
    return nullptr;
}

//------------------------------------------------------------------------------
bool
Registry::HasResourceById(const Id& id) const {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    
    return this->idIndexMap.Contains(id);
}

//------------------------------------------------------------------------------
Id
Registry::LookupResource(const Locator& loc) {
    o_assert(this->isValid);
    if (loc.IsShared()) {
        const Entry* entry = this->findEntryByLocator(loc);
        if (nullptr != entry) {
            this->incrUseCount(entry->id);
            return entry->id;
        }
    }
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
int32
Registry::ReleaseResource(const Id& id, Array<Id>& outRemoved) {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    
    outRemoved.Clear();
    this->decrUseCount(id, outRemoved);
    for (const Id& id : outRemoved) {
        const int32 mapIndex = this->idIndexMap.FindIndex(id);
        if (InvalidIndex != mapIndex) {
            const int32 entryIndex = this->idIndexMap.ValueAtIndex(mapIndex);
            o_assert(this->entries[entryIndex].useCount == 0);
            o_assert(this->entries[entryIndex].id == id);
            Locator loc = this->entries[entryIndex].locator;
            
            // remove entries
            this->entries.EraseSwapBack(entryIndex);
            this->idIndexMap.Erase(id);
            if (loc.IsShared()) {
                this->locatorIndexMap.Erase(loc);
            }
            
            // fixup the index maps (see elementBuffer)
            const int32 swappedIndex = this->entries.Size();
            if (entryIndex != swappedIndex) {
                for (auto& elm : this->idIndexMap) {
                    if (swappedIndex == elm.value) {
                        elm.value = entryIndex;
                        break;
                    }
                }
                for (auto& elm : this->locatorIndexMap) {
                    if (swappedIndex == elm.value) {
                        elm.value = entryIndex;
                        break;
                    }
                }
            }
            
            // make sure nothing broke
            o_assert_dbg(this->checkIntegrity());
        }
    }
    return outRemoved.Size();
}

//------------------------------------------------------------------------------
void
Registry::incrUseCount(const Id& id) {
    o_assert(id.IsValid());
    Entry* entry = (Entry*) this->findEntryById(id);
    o_assert(nullptr != entry);

    // increment ref-count of dependent resources
    for (int32 i = 0; i < entry->numDeps; i++) {
        this->incrUseCount(entry->deps[i]);
    }
    entry->useCount++;
}

//------------------------------------------------------------------------------
int32
Registry::decrUseCount(const Id& id, Array<Id>& outToRemove) {
    o_assert(id.IsValid());
    Entry* entry = (Entry*) this->findEntryById(id);
    o_assert(nullptr != entry);
    o_assert(entry->useCount > 0);
    
    entry->useCount--;
    if (0 == entry->useCount) {
        outToRemove.Add(id);
    }
    
    // recursively decrement use-count of dependents
    for (int32 i = 0; i < entry->numDeps; i++) {
        this->decrUseCount(entry->deps[i], outToRemove);
    }
    return outToRemove.Size();
}

//------------------------------------------------------------------------------
int32
Registry::GetUseCount(const Id& id) const {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    
    const Entry* entry = this->findEntryById(id);
    o_assert(nullptr != entry);
    return entry->useCount;
}

//------------------------------------------------------------------------------
const Locator&
Registry::GetLocator(const Id& id) const {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    
    const Entry* entry = this->findEntryById(id);
    o_assert(nullptr != entry);
    return entry->locator;
}

//------------------------------------------------------------------------------
Array<Id>
Registry::GetDependents(const Id& id) const {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    
    const Entry* entry = this->findEntryById(id);
    o_assert(nullptr != entry);
    Array<Id> result;
    result.Reserve(entry->numDeps);
    for (int32 i = 0; i < entry->numDeps; i++) {
        result.Add(entry->deps[i]);
    }
    return result;
}

//------------------------------------------------------------------------------
int32
Registry::GetNumResources() const {
    o_assert(this->isValid);
    return this->entries.Size();
}

//------------------------------------------------------------------------------
Id
Registry::GetIdByIndex(int32 index) const {
    o_assert(this->isValid);
    return this->entries[index].id;
}

//------------------------------------------------------------------------------
#if ORYOL_DEBUG
bool
Registry::checkIntegrity() const {
    for (const auto& kvp : this->locatorIndexMap) {
        const Locator& loc = kvp.key;
        const int32 entryIndex = kvp.value;
        const Locator& entryLoc = this->entries[entryIndex].locator;
        if (entryLoc != loc) {
            o_error("Resource::Registry: locator mismatch at index '%d' (%s != %s)\n",
                    entryIndex, entryLoc.Location().AsCStr(), loc.Location().AsCStr());
            return false;
        }
    }
    for (const auto& kvp : this->idIndexMap) {
        const Id& id = kvp.key;
        const int32 entryIndex = kvp.value;
        const Id& entryId = this->entries[entryIndex].id;
        if (entryId != id) {
            o_error("Resource::Registry:: id mismatch at index '%d' (%d,%d,%d != %d,%d,%d)\n",
                    entryIndex, entryId.UniqueStamp(), entryId.SlotIndex(), entryId.Type(),
                    id.UniqueStamp(), id.SlotIndex(), id.Type());
            return false;
        }
    }
    return true;
}
#endif

} // namespace Render
} // namespace Oryol