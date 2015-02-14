#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::assetRegistry
    @ingroup _priv
    @brief registry for shared, use-counted assets
    
    The assetRegistry maps asset names (Locators) to use-counted
    AssetIds.
*/
#include <functional>
#include "Resource/Id.h"
#include "Resource/Locator.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Map.h"

namespace Oryol {
namespace _priv {
    
class assetRegistry {
public:
    /// discard function definition
    typedef std::function<void(const Id&)> DiscardFunc;

    /// constructor
    assetRegistry();
    /// destructor
    ~assetRegistry();
    
    /// setup the registry with an estimated number of entries
    void Setup(int32 reserveSize);
    /// discard the registry
    void Discard();
    /// return true if the registry has been setup
    bool IsValid() const;
    
    /// add a new asset to the registry
    void Add(const Locator& loc, const Id& id, DiscardFunc discardFunc);
    /// lookup asset by locator
    Id Lookup(const Locator& loc);
    /// increment the use-count of the asset
    void Use(const Id& id);
    /// decreases use-count, return true if resource must be destroyed
    bool Release(const Id& id);
    
    /// check if asset is registered by AssetId
    bool Exists(const Id& id) const;
    /// (debug) get the current use-count of an asset (fail hard if resource doesn't exist)
    int32 GetUseCount(const Id& id) const;
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
        Entry() :
            useCount(0) {
        };
        Entry(const Locator& loc_, const Id& id_, DiscardFunc discardFunc_) :
            useCount(0),
            locator(loc_),
            id(id_),
            discardFunc(discardFunc_) {
        };
        
        int32 useCount;
        Locator locator;
        Id id;
        DiscardFunc discardFunc;
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
