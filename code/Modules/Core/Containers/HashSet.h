#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::HashSet
    @brief a Set using hashing for fast access
    
    Implements a hash set with a fixed number of buckets, each
    bucket is a binary-sorted set.
    
    @see Array, ArrayMap, Map, Set
*/
#include "Core/Config.h"
#include "Core/Containers/Set.h"

namespace Oryol {
namespace Core {

template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> class HashSet {
public:
    /// default constructor
    HashSet();
    /// copy constructor
    HashSet(const HashSet& rhs);
    /// move constructor
    HashSet(HashSet&& rhs);
    /// copy-assignment operator
    void operator=(const HashSet& rhs);
    /// move-assignment operator (same capacity and size)
    void operator=(HashSet&& rhs);
    
    /// set allocation strategy
    void SetAllocStrategy(int32 minGrow, int32 maxGrow=ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    /// get min grow value
    int32 GetMinGrow() const;
    /// get max grow value
    int32 GetMaxGrow() const;
    /// get number of elements in array
    int32 Size() const;
    /// return true if empty
    bool Empty() const;
    
    /// test if an element exists
    bool Contains(const VALUETYPE& val) const;
    /// find element
    const VALUETYPE* Find(const VALUETYPE& val) const;
    /// insert element
    void Insert(const VALUETYPE& val);
    /// erase element
    void Erase(const VALUETYPE& val);
    
private:
    /// get the bucket for a value
    const Set<VALUETYPE>& findBucket(const VALUETYPE& val) const;
    /// get the bucket for a value
    Set<VALUETYPE>& findBucket(const VALUETYPE& val);

    int32 size;
    Set<VALUETYPE> buckets[NUMBUCKETS];
};

//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS>
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::HashSet() :
size(0) {
    // empty
};

//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS>
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::HashSet(const HashSet& rhs) :
size(rhs.size) {
    for (int i = 0; i < NUMBUCKETS; i++) {
        this->buckets[i] = rhs.buckets[i];
    };
}

//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS>
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::HashSet(HashSet&& rhs) :
size(rhs.size) {
    for (int i = 0; i < NUMBUCKETS; i++) {
        this->buckets[i] = std::move(rhs.buckets[i]);
    }
    rhs.size = 0;
}

//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> void
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::operator=(const HashSet& rhs) {
    if (&rhs != this) {
        this->size = rhs.size;
        for (int i = 0; i < NUMBUCKETS; i++) {
            this->buckets[i] = rhs.buckets[i];
        }
    }
}

//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> void
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::operator=(HashSet&& rhs) {
    if (&rhs != this) {
        this->size = rhs.size;
        for (int i = 0; i < NUMBUCKETS; i++) {
            this->buckets[i] = std::move(rhs.buckets[i]);
        }
        rhs.size = 0;
    }
}
    
//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> void
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::SetAllocStrategy(int32 minGrow, int32 maxGrow) {
    for (int i = 0; i < NUMBUCKETS; i++) {
        this->buckets[i].SetAllocStrategy(minGrow, maxGrow);
    }
}

//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> int32
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::GetMinGrow() const {
    return this->buckets[0].GetMinGrow();
}

//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> int32
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::GetMaxGrow() const {
    return this->buckets[0].GetMaxGrow();
}

//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> int32
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::Size() const {
    return this->size;
}

//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> bool
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::Empty() const {
    return (0 == this->size);
}
    
//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> bool
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::Contains(const VALUETYPE& val) const {
    const auto& bucket = this->findBucket(val);
    return bucket.Contains(val);
}
    
//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> const VALUETYPE*
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::Find(const VALUETYPE& val) const {
    const auto& bucket = this->findBucket(val);
    return bucket.Find(val);
}
    
//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> void
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::Insert(const VALUETYPE& val) {
    this->size++;
    auto& bucket = this->findBucket(val);
    bucket.Insert(val);
}
    
//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> void
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::Erase(const VALUETYPE& val) {
    o_assert_dbg(this->size > 0);
    this->size--;
    auto& bucket = this->findBucket(val);
    bucket.Erase(val);
};
    
//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> const Set<VALUETYPE>&
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::findBucket(const VALUETYPE& val) const {
    return this->buckets[uint32(HASHER()(val)) % NUMBUCKETS];
}

//------------------------------------------------------------------------------
template<class VALUETYPE, class HASHER, int32 NUMBUCKETS> Set<VALUETYPE>&
HashSet<VALUETYPE, HASHER, NUMBUCKETS>::findBucket(const VALUETYPE& val) {
    return this->buckets[uint32(HASHER()(val)) % NUMBUCKETS];
}

} // namespace Core
} // namespace Oryol