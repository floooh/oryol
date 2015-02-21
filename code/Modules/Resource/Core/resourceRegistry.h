#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol:resourceRegistry
    @ingroup _priv
    @brief registry for shared, use-counted assets
    
    The resourceRegistry maps resource names (Locators) to use-counted
    resource Ids.
*/
#include "Resource/Id.h"
#include "Resource/Locator.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Map.h"

namespace Oryol {
namespace _priv {
    
class resourceRegistry {
public:
    /// constructor
    resourceRegistry();
    /// destructor
    ~resourceRegistry();
    
    /// setup the registry with an estimated number of entries
    void Setup(int32 reserveSize);
    /// discard the registry
    void Discard();
    /// return true if the registry has been setup
    bool IsValid() const;
    
    /// add a new asset to the registry
    void Add(const Locator& loc, const Id& id);
    /// lookup asset by locator
    Id Lookup(const Locator& loc) const;
    /// remove all resource matching label from registry, returns removed Ids
    Array<Id> Remove(uint8 label);
    
    /// check if asset is registered by AssetId
    bool Contains(const Id& id) const;
    /// (debug) get the locator of an asset (fail hard if asset doesn't exist)
    const Locator& GetLocator(const Id& id) const;
    
    /// (debug) get number of assets in the registry
    int32 GetNumResources() const;
    /// (debug) get asset id by index
    Id GetIdByIndex(int32 index) const;
    
private:
    /// increment use count of asset
    void incrUseCount(const Id& id);
    /// decrement use count of resource and dependents, return 0-usecount resources
    bool decrUseCount(const Id& id);
    #if ORYOL_DEBUG
    /// validate integrity of internal data structures
    bool checkIntegrity() const;
    #endif
    
    struct Entry {
        Entry(const Locator& loc_, const Id& id_) :
            locator(loc_),
            id(id_) { };
        
        Locator locator;
        Id id;
    };
    
    /// find an entry by locator
    const Entry* findEntryByLocator(const Locator& loc) const;
    /// find an entry by id
    const Entry* findEntryById(const Id& id) const;
    
    bool isValid;
    Array<Entry> entries;
    Map<Locator, int32> locatorIndexMap;
    Map<Id, int32> idIndexMap;
};
} // namespace _priv
} // namespace Oryol
