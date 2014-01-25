#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::HashSet
    
    Implements a HashMap with a fixed number of buckets, each 
    bucket is a binary-sorted set.
*/
#include "Core/Config.h"
#include "Core/Containers/Set.h"

namespace Oryol {
namespace Core {

template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> class HashSet {
public:
    /// default constructor
    HashSet() :
        size(0) {
    };
    /// construct with allocation strategy
    HashSet(int32 minGrow, int32 maxGrow=ORYOL_CONTAINER_DEFAULT_MAX_GROW) :
        size(0) {
        this->SetAllocStrategy(minGrow, maxGrow);
    };
    /// copy constructor
    HashSet(const HashSet& rhs) :
        size(rhs.size) {
        for (int i = 0; i < NUMBUCKETS; i++) {
            this->buckets[i] = rhs.buckets[i];
        };
    };
    /// move constructor
    HashSet(HashSet&& rhs) :
        size(rhs.size) {
        for (int i = 0; i < NUMBUCKETS; i++) {
            this->buckets[i] = std::move(rhs.buckets[i]);
        }
        rhs.size = 0;
    };
    /// copy-assignment operator
    void operator=(const HashSet& rhs) {
        this->size = rhs.size;
        for (int i = 0; i < NUMBUCKETS; i++) {
            this->buckets[i] = rhs.buckets[i];
        }
    };
    /// move-assignment operator (same capacity and size)
    void operator=(HashSet&& rhs) {
        this->size = rhs.size;
        for (int i = 0; i < NUMBUCKETS; i++) {
            this->buckets[i] = std::move(rhs.buckets[i]);
        }
        rhs.size = 0;
    };
    
    /// set allocation strategy
    void SetAllocStrategy(int32 minGrow, int32 maxGrow=ORYOL_CONTAINER_DEFAULT_MAX_GROW) {
        for (int i = 0; i < NUMBUCKETS; i++) {
            this->buckets[i].SetAllocStrategy(minGrow, maxGrow);
        }
    };
    /// get min grow value
    int32 GetMinGrow() const {
        return this->buckets[0].GetMinGrow();
    };
    /// get max grow value
    int32 GetMaxGrow() const {
        return this->buckets[0].GetMaxGrow();
    };
    /// get number of elements in array
    int32 Size() const {
        return this->size;
    };
    /// return true if empty
    bool Empty() const {
        return (0 == this->size);
    };
    
    /// test if an element exists
    bool Contains(const VALUETYPE& val) const {
        const auto& bucket = this->findBucket(val);
        return bucket.Contains(val);
    };
    
    /// find element
    const VALUETYPE* Find(const VALUETYPE& val) const {
        const auto& bucket = this->findBucket(val);
        return bucket.Find(val);
    };
    
    /// insert element
    void Insert(const VALUETYPE& val) {
        this->size++;
        auto& bucket = this->findBucket(val);
        bucket.Insert(val);
    };
    
    /// erase element
    void Erase(const VALUETYPE& val) {
        o_assert(this->size > 0);
        this->size--;
        auto& bucket = this->findBucket(val);
        bucket.Erase(val);
    };
    
private:
    /// get the bucket for a value
    const Set<VALUETYPE>& findBucket(const VALUETYPE& val) const {
        return this->buckets[uint32(HASHER()(val)) % NUMBUCKETS];
    };
    /// get the bucket for a value
    Set<VALUETYPE>& findBucket(const VALUETYPE& val) {
        return this->buckets[uint32(HASHER()(val)) % NUMBUCKETS];
    };

    int32 size;
    Set<VALUETYPE> buckets[NUMBUCKETS];
};
    
} // namespace Core
} // namespace Oryol