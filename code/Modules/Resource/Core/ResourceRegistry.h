#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourceRegistry
    @ingroup Resource
    @brief implements use-counted resource-sharing
*/
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Map.h"

namespace Oryol {
    
class ResourceRegistry {
public:
    /// constructor
    ResourceRegistry();
    /// destructor
    ~ResourceRegistry();
    
    /// setup the registry with an estimated number of entries
    void Setup(int32 reserveSize);
    /// discard the registry
    void Discard();
    /// return true if the registry has been setup
    bool IsValid() const;
    
    /// add a new resource id to the registry
    void AddResource(const Locator& loc, const Id& id);
    /// lookup resource by locator
    Id LookupResource(const Locator& loc);
    /// increment the use-count of the resource (and its dependents)
    void UseResource(const Id& id);
    /// decreases use-count, return true if resource must be destroyed
    bool ReleaseResource(const Id& id);
    
    /// check if resource is registered by resource id
    bool HasResourceById(const Id& id) const;
    /// (debug) get the current use-count of a resource (fail hard if resource doesn't exist)
    int32 GetUseCount(const Id& id) const;
    /// (debug) get the locator of a resource (fail hard if resource doesn't exist)
    const Locator& GetLocator(const Id& id) const;
    
    /// (debug) get number of resource in the registry
    int32 GetNumResources() const;
    /// (debug) get resource id by index
    Id GetIdByIndex(int32 index) const;
    
private:
    /// increment use count of resource and dependents
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
        Entry(const Locator& loc_, const Id& id_) :
            useCount(0),
            locator(loc_),
            id(id_) {
        };
        
        int32 useCount;
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
    
} // namespace Oryol
