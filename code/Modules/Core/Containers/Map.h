#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Map
    @brief simple key-value map similar to std::map
    
    A key-value-pair container similar to std::map, with the following
    differences:
    
    - trying to access a non-existing element with operator[] will
      trigger an assertion instead of adding it
    
    Map is useful when the values type is small and has a cheap copy/move
    operation. For big value and/or complex value types, consider using
    an ArrayMap!
 
    Allows to add multiple elements with the same key, these will be lumped 
    together in the internal buffer and will have adjacent indices
      
    You can use the FindDuplicate() to check for redundant elements,
    this is O(N) though.
      
    When adding large numbers of elements, consider using the 
    bulk methods, these destroy the sorted order when inserting,
    and sorting will happen inside EndBulk().
    
    The Map uses a double-ended element buffer internally which
    initially has spare room at the front and end. When inserting elements,
    movement happens towards the end which would create less move operations
    (so inserting at the front is just as fast as inserting at the end).
    
    @see KeyValuePair, Set
*/
#include <algorithm>
#include "Core/Config.h"
#include "Core/Containers/elementBuffer.h"
#include "Core/Containers/KeyValuePair.h"

namespace Oryol {
namespace Core {

template<class KEY, class VALUE> class Map {
public:
    /// default constructor
    Map();
    /// copy constructor (truncates to actual size)
    Map(const Map& rhs);
    /// move constructor (same capacity and size)
    Map(Map&& rhs);
    /// destructor
    ~Map();
    
    /// copy-assignment operator (truncates to actual size)
    void operator=(const Map& rhs);
    /// move-assignment operator (same capacity and size)
    void operator=(Map&& rhs);
    
    /// set allocation strategy
    void SetAllocStrategy(int32 minGrow_, int32 maxGrow_=ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    /// get min grow value
    int32 GetMinGrow() const;
    /// get max grow value
    int32 GetMaxGrow() const;
    /// get number of elements in array
    int32 Size() const;
    /// return true if empty
    bool Empty() const;
    /// get capacity of array
    int32 Capacity() const;

    /// read/write access single element
    VALUE& operator[](const KEY& key);
    /// read-only access single element
    const VALUE& operator[](const KEY& key) const;
    
    /// increase capacity to hold at least numElements more elements
    void Reserve(int32 numElements);
    /// trim capacity to size (this involves a re-alloc)
    void Trim();
    /// clear the array (deletes elements, keeps capacity)
    void Clear();

    /// test if an element exists
    bool Contains(const KEY& key) const;
    /// insert new element
    void Insert(const KeyValuePair<KEY, VALUE>& kvp);
    /// insert new element
    void Insert(KeyValuePair<KEY, VALUE>&& kvp);
    /// insert new element
    void Insert(const KEY& key, const VALUE& value);
    /// insert new element, return false if element with key already existed
    bool InsertUnique(const KeyValuePair<KEY, VALUE>& kvp);
    /// insert new element with move-semantics, return false if element with key already existed
    bool InsertUnique(KeyValuePair<KEY, VALUE>&& kvp);
    /// insert new element, and check that it is unique
    bool InsertUnique(const KEY& key, const VALUE& value);
    /// erase all elements matching key, does nothing if key not contained
    void Erase(const KEY& key);
    
    /// begin bulk-mode
    void BeginBulk();
    /// insert element in bulk-mode (destroys sorting order)
    void InsertBulk(const KeyValuePair<KEY, VALUE>& kvp);
    /// insert element in bulk-mode (destroys sorting order)
    void InsertBulk(KeyValuePair<KEY, VALUE>&& kvp);
    /// insert element in bulk-mode (destroys sorting order)
    void InsertBulk(const KEY& key, const VALUE& value);
    /// end bulk-mode (sorting happend here)
    void EndBulk();
    /// find the first duplicate element, or InvalidIndex if not found, this is O(N)!
    int32 FindDuplicate(int32 startIndex) const;
    /// find an element, returns index, or InvalidIndex
    int32 FindIndex(const KEY& key) const;
    /// erase element at index
    void EraseIndex(int32 index);
    /// get key at index
    const KEY& KeyAtIndex(int32 index) const;
    /// get value at index (read-only)
    const VALUE& ValueAtIndex(int32 index) const;
    /// get value at index (read/write)
    VALUE& ValueAtIndex(int32 index);
    
    /// C++ conform begin, MAY RETURN nullptr!
    KeyValuePair<KEY, VALUE>* begin();
    /// C++ conform begin, MAY RETURN nullptr!
    const KeyValuePair<KEY, VALUE>* begin() const;
    /// C++ conform end,  MAY RETURN nullptr!
    KeyValuePair<KEY, VALUE>* end();
    /// C++ conform end, MAY RETURN nullptr!
    const KeyValuePair<KEY, VALUE>* end() const;
    
private:
    /// destroy content
    void destroy();
    /// copy content
    void copy(const Map& rhs);
    /// move content
    void move(Map&& rhs);
    /// reallocate with new capacity
    void adjustCapacity(int32 newCapacity);
    /// grow to make room
    void grow();
    
    elementBuffer<KeyValuePair<KEY,VALUE>> buffer;
    int32 minGrow;
    int32 maxGrow;
    bool inBulkMode;
};
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE>
Map<KEY, VALUE>::Map() :
minGrow(ORYOL_CONTAINER_DEFAULT_MIN_GROW),
maxGrow(ORYOL_CONTAINER_DEFAULT_MAX_GROW),
inBulkMode(false) {
    // empty
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE>
Map<KEY, VALUE>::Map(const Map& rhs) {
    this->copy(rhs);
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE>
Map<KEY, VALUE>::Map(Map&& rhs) {
    this->move(std::move(rhs));
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE>
Map<KEY, VALUE>::~Map() {
    this->destroy();
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::operator=(const Map& rhs) {
    /// @todo: this should be optimized when rhs.size() < this->capacity()!
    if (&rhs != this) {
        this->destroy();
        this->copy(rhs);
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::operator=(Map&& rhs) {
    /// @todo: this should be optimized when rhs.size() < this->capacity()!
    if (&rhs != this) {
        this->destroy();
        this->move(std::move(rhs));
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::SetAllocStrategy(int32 minGrow_, int32 maxGrow_) {
    this->minGrow = minGrow_;
    this->maxGrow = maxGrow_;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int32
Map<KEY, VALUE>::GetMinGrow() const {
    return this->minGrow;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int32
Map<KEY, VALUE>::GetMaxGrow() const {
    return this->maxGrow;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int32
Map<KEY, VALUE>::Size() const {
    return this->buffer.size();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
Map<KEY, VALUE>::Empty() const {
    return this->buffer.elmStart == this->buffer.elmEnd;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int32
Map<KEY, VALUE>::Capacity() const {
    return this->buffer.capacity();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
Map<KEY, VALUE>::Contains(const KEY& key) const {
    o_assert(!this->inBulkMode);
    return std::binary_search(this->buffer.elmStart, this->buffer.elmEnd, key);
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> VALUE&
Map<KEY, VALUE>::operator[](const KEY& key) {
    o_assert(!this->inBulkMode);
    auto kvp = std::lower_bound(this->buffer.elmStart, this->buffer.elmEnd, key);
    o_assert((kvp != this->buffer.elmEnd) && (key == kvp->key));    // not found if this triggers
    return kvp->value;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const VALUE&
Map<KEY, VALUE>::operator[](const KEY& key) const {
    o_assert(!this->inBulkMode);
    auto kvp = std::lower_bound(this->buffer.elmStart, this->buffer.elmEnd, key);
    o_assert((kvp != this->buffer.elmEnd) && (key == kvp->key));    // not found if this triggers
    return kvp->value;
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::Reserve(int32 numElements) {
    int32 newCapacity = this->buffer.size() + numElements;
    if (newCapacity > this->buffer.capacity()) {
        this->adjustCapacity(newCapacity);
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::Trim() {
    const int32 curSize = this->buffer.size();
    if (curSize < this->buffer.capacity()) {
        this->adjustCapacity(curSize);
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::Clear() {
    this->buffer.clear();
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::Insert(const KeyValuePair<KEY, VALUE>& kvp) {
    o_assert(!this->inBulkMode);
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    auto ptr = std::lower_bound(this->buffer.elmStart, this->buffer.elmEnd, kvp.key);
    int32 index = ptr - this->buffer.elmStart;
    this->buffer.insert(index, kvp);
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::Insert(KeyValuePair<KEY, VALUE>&& kvp) {
    o_assert(!this->inBulkMode);
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    auto ptr = std::lower_bound(this->buffer.elmStart, this->buffer.elmEnd, kvp.key);
    int32 index = ptr - this->buffer.elmStart;
    this->buffer.insert(index, std::move(kvp));
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::Insert(const KEY& key, const VALUE& value) {
    this->Insert(KeyValuePair<KEY, VALUE>(key, value));
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
Map<KEY, VALUE>::InsertUnique(const KeyValuePair<KEY, VALUE>& kvp) {
    o_assert(!this->inBulkMode);
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    auto ptr = std::lower_bound(this->buffer.elmStart, this->buffer.elmEnd, kvp.key);
    if ((ptr != this->buffer.elmEnd) && (ptr->key == kvp.key)) {
        return false;
    }
    else {
        int32 index = ptr - this->buffer.elmStart;
        this->buffer.insert(index, kvp);
        return true;
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
Map<KEY, VALUE>::InsertUnique(KeyValuePair<KEY, VALUE>&& kvp) {
    o_assert(!this->inBulkMode);
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    auto ptr = std::lower_bound(this->buffer.elmStart, this->buffer.elmEnd, kvp.key);
    if ((ptr != this->buffer.elmEnd) && (ptr->key == kvp.key)) {
        return false;
    }
    else {
        int32 index = ptr - this->buffer.elmStart;
        this->buffer.insert(index, std::move(kvp));
        return true;
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
Map<KEY, VALUE>::InsertUnique(const KEY& key, const VALUE& value) {
    return this->InsertUnique(KeyValuePair<KEY, VALUE>(key, value));
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::Erase(const KEY& key) {
    auto ptr = std::lower_bound(this->buffer.elmStart, this->buffer.elmEnd, key);
    if (ptr != this->buffer.elmEnd) {
        const int32 index = ptr - this->buffer.elmStart;
        while ((index < this->buffer.size()) && (this->buffer[index].key == key)) {
            this->buffer.erase(index);
        }
    }
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::BeginBulk() {
    o_assert(!this->inBulkMode);
    this->inBulkMode = true;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::InsertBulk(const KeyValuePair<KEY, VALUE>& kvp) {
    o_assert(this->inBulkMode);
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    // keep frontSpare and backSpare balanced
    if (this->buffer.frontSpare() > this->buffer.backSpare()) {
        // insert at front
        this->buffer.insert(0, kvp);
    }
    else {
        // insert at back
        this->buffer.insert(this->buffer.size(), kvp);
    }
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::InsertBulk(KeyValuePair<KEY, VALUE>&& kvp) {
    o_assert(this->inBulkMode);
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    // keep frontSpare and backSpare balanced
    if (this->buffer.frontSpare() > this->buffer.backSpare()) {
        // insert at front
        this->buffer.insert(0, std::move(kvp));
    }
    else {
        // insert at back
        this->buffer.insert(this->buffer.size(), std::move(kvp));
    }
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::InsertBulk(const KEY& key, const VALUE& value) {
    this->Insert(KeyValuePair<KEY, VALUE>(key, value));
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::EndBulk() {
    o_assert(this->inBulkMode);
    this->inBulkMode = false;
    std::sort(this->buffer.elmStart, this->buffer.elmEnd);
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int32
Map<KEY, VALUE>::FindDuplicate(int32 startIndex) const {
    o_assert(!this->inBulkMode);
    const int32 size = this->buffer.size();
    if (startIndex < size) {
        for (int32 index = startIndex; index < (size - 1); index++) {
            if (this->buffer[index].key == this->buffer[index + 1].key) {
                return index;
            }
        }
    }
    return InvalidIndex;
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> int32
Map<KEY, VALUE>::FindIndex(const KEY& key) const {
    o_assert(!this->inBulkMode);
    auto ptr = std::lower_bound(this->buffer.elmStart, this->buffer.elmEnd, key);
    if ((ptr != this->buffer.elmEnd) && (key == ptr->key)) {
        return ptr - this->buffer.elmStart;
    }
    else {
        return InvalidIndex;
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::EraseIndex(int32 index) {
    this->buffer.erase(index);
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const KEY&
Map<KEY, VALUE>::KeyAtIndex(int32 index) const {
    return this->buffer[index].key;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const VALUE&
Map<KEY, VALUE>::ValueAtIndex(int32 index) const {
    return this->buffer[index].value;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> VALUE&
Map<KEY, VALUE>::ValueAtIndex(int32 index) {
    return this->buffer[index].value;
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> KeyValuePair<KEY, VALUE>*
Map<KEY, VALUE>::begin() {
    return this->buffer.elmStart;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const KeyValuePair<KEY, VALUE>*
Map<KEY, VALUE>::begin() const {
    return this->buffer.elmStart;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> KeyValuePair<KEY, VALUE>*
Map<KEY, VALUE>::end() {
    return this->buffer.elmEnd;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const KeyValuePair<KEY, VALUE>*
Map<KEY, VALUE>::end() const {
    return this->buffer.elmEnd;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::destroy() {
    this->minGrow = 0;
    this->maxGrow = 0;
    this->buffer.destroy();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::copy(const Map& rhs) {
    this->minGrow    = rhs.minGrow;
    this->maxGrow    = rhs.maxGrow;
    this->inBulkMode = rhs.inBulkMode;
    this->buffer     = rhs.buffer;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::move(Map&& rhs) {
    o_assert(!rhs.inBulkMode);
    this->minGrow    = rhs.minGrow;
    this->maxGrow    = rhs.maxGrow;
    this->inBulkMode = rhs.inBulkMode;
    this->buffer  = std::move(rhs.buffer);
    // NOTE: don't reset minGrow/maxGrow, rhs is empty, but still a valid object!
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::adjustCapacity(int32 newCapacity) {
    // have a balanced front and back spare
    int32 frontSpare = (newCapacity - this->buffer.size()) >> 1;
    o_assert(frontSpare >= 0);
    this->buffer.alloc(newCapacity, frontSpare);
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::grow() {
    const int32 curCapacity = this->buffer.capacity();
    int growBy = curCapacity >> 1;
    if (growBy < minGrow) {
        growBy = minGrow;
    }
    else if (growBy > maxGrow) {
        growBy = maxGrow;
    }
    o_assert(growBy > 0);
    int newCapacity = curCapacity + growBy;
    this->adjustCapacity(newCapacity);
}

} // namespace Core
} // namespace Oryol
