#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Resource::Registry
    @brief implements use-counted resource-sharing
*/
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Map.h"

namespace Oryol {
namespace Resource {
    
class Registry {
public:
    /// constructor
    Registry();
    /// destructor
    ~Registry();
    
    /// setup the registry with an estimated number of entries
    void Setup(int32 reserveSize);
    /// discard the registry
    void Discard();
    /// return true if the registry has been setup
    bool IsValid() const;
    
    /// add a new resource id to the registry
    void AddResource(const Locator& loc, const Id& id);
    /// add a new resource id with dependents
    void AddResource(const Locator& loc, const Id& id, const Array<Id>& deps);
    /// lookup resource by locator, will increment the use-count of the resource!
    Id LookupResource(const Locator& loc);
    /// decreases use-count, and returns all resources which have reached use-count 0
    int32 ReleaseResource(const Id& id, Array<Id>& outRemoved);
    
    /// check if resource is registered by resource id
    bool HasResourceById(const Id& id) const;
    /// (debug) get the current use-count of a resource (fail hard if resource doesn't exist)
    int32 GetUseCount(const Id& id) const;
    /// (debug) get the locator of a resource (fail hard if resource doesn't exist)
    const Locator& GetLocator(const Id& id) const;
    /// (debug) get dependents of a resource
    Array<Id> GetDependents(const Id& id) const;
    
    /// (debug) get number of resource in the registry
    int32 GetNumResources() const;
    /// (debug) get resource id by index
    Id GetIdByIndex(int32 index) const;
    
private:
    /// increment use count of resource and dependents
    void incrUseCount(const Id& id);
    /// decrement use count of resource and dependents, return 0-usecount resources
    int32 decrUseCount(const Id& id, Array<Id>& outToRemove);
    #if ORYOL_DEBUG
    /// validate integrity of internal data structures
    bool checkIntegrity() const;
    #endif
    
    /// max number of dependents
    static const int32 MaxNumDependents = 8;
    
    struct Entry {
        Entry() :
            useCount(0),
            numDeps(0) {
        };
        Entry(const Locator& loc_, const Id& id_) :
            useCount(0),
            locator(loc_),
            id(id_),
            numDeps(0) {
        };
        
        int32 useCount;
        Locator locator;
        Id id;
        int32 numDeps;
        Id deps[MaxNumDependents];
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
    
} // namespace Resource
} // namespace Oryol
