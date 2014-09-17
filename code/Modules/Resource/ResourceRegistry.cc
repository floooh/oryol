//------------------------------------------------------------------------------
//  Registry.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ResourceRegistry.h"

namespace Oryol {

//------------------------------------------------------------------------------
ResourceRegistry::ResourceRegistry() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
ResourceRegistry::~ResourceRegistry() {
    o_assert(!this->isValid);
    o_assert(this->entries.Empty());
}

//------------------------------------------------------------------------------
void
ResourceRegistry::Setup(int32 reserveSize) {
    o_assert(!this->isValid);
    
    this->isValid = true;
    this->entries.Reserve(reserveSize);
    this->locatorIndexMap.Reserve(reserveSize);
    this->idIndexMap.Reserve(reserveSize);
}

//------------------------------------------------------------------------------
void
ResourceRegistry::Discard() {
    o_assert(this->isValid);
    
    this->entries.Clear();
    this->locatorIndexMap.Clear();
    this->idIndexMap.Clear();
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
ResourceRegistry::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
ResourceRegistry::AddResource(const Locator& loc, const Id& id) {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    o_assert(!this->idIndexMap.Contains(id));
    
    this->entries.Emplace(loc, id);
    if (loc.IsShared()) {
        o_assert(!this->locatorIndexMap.Contains(loc));
        this->locatorIndexMap.Add(loc, this->entries.Size() - 1);
    }
    this->idIndexMap.Add(id, this->entries.Size() - 1);
}

//------------------------------------------------------------------------------
const ResourceRegistry::Entry*
ResourceRegistry::findEntryByLocator(const Locator& loc) const {
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
const ResourceRegistry::Entry*
ResourceRegistry::findEntryById(const Id& id) const {
    const int32 mapIndex = this->idIndexMap.FindIndex(id);
    if (InvalidIndex != mapIndex) {
        const int32 entryIndex = this->idIndexMap.ValueAtIndex(mapIndex);
        return &(this->entries[entryIndex]);
    }
    return nullptr;
}

//------------------------------------------------------------------------------
bool
ResourceRegistry::HasResourceById(const Id& id) const {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    
    return this->idIndexMap.Contains(id);
}

//------------------------------------------------------------------------------
Id
ResourceRegistry::LookupResource(const Locator& loc) {
    o_assert(this->isValid);
    if (loc.IsShared()) {
        const Entry* entry = this->findEntryByLocator(loc);
        if (nullptr != entry) {
            return entry->id;
        }
    }
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
void
ResourceRegistry::UseResource(const Id& id) {
    this->incrUseCount(id);
}

//------------------------------------------------------------------------------
bool
ResourceRegistry::ReleaseResource(const Id& id) {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    
    if (this->decrUseCount(id)) {
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
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
ResourceRegistry::incrUseCount(const Id& id) {
    o_assert_dbg(id.IsValid());
    Entry* entry = (Entry*) this->findEntryById(id);
    o_assert_dbg(nullptr != entry);
    entry->useCount++;
}

//------------------------------------------------------------------------------
bool
ResourceRegistry::decrUseCount(const Id& id) {
    o_assert_dbg(id.IsValid());
    Entry* entry = (Entry*) this->findEntryById(id);
    o_assert_dbg(nullptr != entry);
    o_assert_dbg(entry->useCount > 0);
    entry->useCount--;
    return 0 == entry->useCount;
}

//------------------------------------------------------------------------------
int32
ResourceRegistry::GetUseCount(const Id& id) const {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    
    const Entry* entry = this->findEntryById(id);
    o_assert(nullptr != entry);
    return entry->useCount;
}

//------------------------------------------------------------------------------
const Locator&
ResourceRegistry::GetLocator(const Id& id) const {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    
    const Entry* entry = this->findEntryById(id);
    o_assert(nullptr != entry);
    return entry->locator;
}

//------------------------------------------------------------------------------
int32
ResourceRegistry::GetNumResources() const {
    o_assert(this->isValid);
    return this->entries.Size();
}

//------------------------------------------------------------------------------
Id
ResourceRegistry::GetIdByIndex(int32 index) const {
    o_assert(this->isValid);
    return this->entries[index].id;
}

//------------------------------------------------------------------------------
#if ORYOL_DEBUG
bool
ResourceRegistry::checkIntegrity() const {
    for (const auto& kvp : this->locatorIndexMap) {
        const Locator& loc = kvp.key;
        const int32 entryIndex = kvp.value;
        const Locator& entryLoc = this->entries[entryIndex].locator;
        if (entryLoc != loc) {
            o_error("ResourceRegistry: locator mismatch at index '%d' (%s != %s)\n",
                    entryIndex, entryLoc.Location().AsCStr(), loc.Location().AsCStr());
            return false;
        }
    }
    for (const auto& kvp : this->idIndexMap) {
        const Id& id = kvp.key;
        const int32 entryIndex = kvp.value;
        const Id& entryId = this->entries[entryIndex].id;
        if (entryId != id) {
            o_error("ResourceRegistry:: id mismatch at index '%d' (%d,%d,%d != %d,%d,%d)\n",
                    entryIndex, entryId.UniqueStamp(), entryId.SlotIndex(), entryId.Type(),
                    id.UniqueStamp(), id.SlotIndex(), id.Type());
            return false;
        }
    }
    return true;
}
#endif

} // namespace Oryol
