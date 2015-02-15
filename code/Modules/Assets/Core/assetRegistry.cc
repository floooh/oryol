//------------------------------------------------------------------------------
//  assetRegistry.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "assetRegistry.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
assetRegistry::assetRegistry() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
assetRegistry::~assetRegistry() {
    o_assert(!this->isValid);
    o_assert(this->entries.Empty());
}

//------------------------------------------------------------------------------
void
assetRegistry::Setup(int32 reserveSize) {
    o_assert(!this->isValid);
    
    this->isValid = true;
    this->entries.Reserve(reserveSize);
    this->locatorIndexMap.Reserve(reserveSize);
    this->idIndexMap.Reserve(reserveSize);
}

//------------------------------------------------------------------------------
void
assetRegistry::Discard() {
    o_assert(this->isValid);
    
    this->entries.Clear();
    this->locatorIndexMap.Clear();
    this->idIndexMap.Clear();
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
assetRegistry::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
assetRegistry::Add(const Locator& loc, const Id& id) {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    o_assert(!this->idIndexMap.Contains(id));
    
    this->entries.Add(loc, id);
    if (loc.IsShared()) {
        o_assert(!this->locatorIndexMap.Contains(loc));
        this->locatorIndexMap.Add(loc, this->entries.Size() - 1);
    }
    this->idIndexMap.Add(id, this->entries.Size() - 1);
}

//------------------------------------------------------------------------------
const assetRegistry::Entry*
assetRegistry::findEntryByLocator(const Locator& loc) const {
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
const assetRegistry::Entry*
assetRegistry::findEntryById(const Id& id) const {
    const int32 mapIndex = this->idIndexMap.FindIndex(id);
    if (InvalidIndex != mapIndex) {
        const int32 entryIndex = this->idIndexMap.ValueAtIndex(mapIndex);
        return &(this->entries[entryIndex]);
    }
    return nullptr;
}

//------------------------------------------------------------------------------
bool
assetRegistry::Contains(const Id& id) const {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    return this->idIndexMap.Contains(id);
}

//------------------------------------------------------------------------------
Id
assetRegistry::Lookup(const Locator& loc) {
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
assetRegistry::Remove(uint8 label) {
    o_assert(this->isValid);
    
    // for each entry where id.label matches label (from behind
    // because matching entries will be removed)
    // FIXME: this can be slow if many resource are live!
    int32 entryIndex = this->entries.Size() - 1;
    for (; entryIndex >= 0; entryIndex--) {
        if (this->entries[entryIndex].id.Label() == label) {
            Id id = this->entries[entryIndex].id;
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
            #if ORYOL_DEBUG
            o_assert(this->checkIntegrity());
            #endif
        }
    }
}

//------------------------------------------------------------------------------
const Locator&
assetRegistry::GetLocator(const Id& id) const {
    o_assert(this->isValid);
    o_assert(id.IsValid());
    
    const Entry* entry = this->findEntryById(id);
    o_assert(nullptr != entry);
    return entry->locator;
}

//------------------------------------------------------------------------------
int32
assetRegistry::GetNumResources() const {
    o_assert(this->isValid);
    return this->entries.Size();
}

//------------------------------------------------------------------------------
Id
assetRegistry::GetIdByIndex(int32 index) const {
    o_assert(this->isValid);
    return this->entries[index].id;
}

//------------------------------------------------------------------------------
#if ORYOL_DEBUG
bool
assetRegistry::checkIntegrity() const {
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

} // namespace _priv
} // namespace Oryol
