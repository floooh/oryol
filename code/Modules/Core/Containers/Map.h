#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Map
    @ingroup Core
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
#include <initializer_list>
#include "Core/Config.h"
#include "Core/Containers/elementBuffer.h"
#include "Core/Containers/KeyValuePair.h"

namespace Oryol {

template<class KEY, class VALUE> class Map {
public:
    /// default constructor
    Map();
    /// copy constructor (truncates to actual size)
    Map(const Map& rhs);
    /// move constructor (same capacity and size)
    Map(Map&& rhs);
    /// construct from initializer list
    Map(std::initializer_list<KeyValuePair<KEY,VALUE>> rhs);
    /// destructor
    ~Map();
    
    /// copy-assignment operator (truncates to actual size)
    void operator=(const Map& rhs);
    /// move-assignment operator (same capacity and size)
    void operator=(Map&& rhs);
    
    /// set allocation strategy
    void SetAllocStrategy(int minGrow_, int maxGrow_=ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    /// get min grow value
    int GetMinGrow() const;
    /// get max grow value
    int GetMaxGrow() const;
    /// get number of elements in array
    int Size() const;
    /// return true if empty
    bool Empty() const;
    /// get capacity of array
    int Capacity() const;

    /// read/write access single element
    VALUE& operator[](const KEY& key);
    /// read-only access single element
    const VALUE& operator[](const KEY& key) const;
    
    /// increase capacity to hold at least numElements more elements
    void Reserve(int numElements);
    /// trim capacity to size (this involves a re-alloc)
    void Trim();
    /// clear the array (deletes elements, keeps capacity)
    void Clear();

    /// test if an element exists
    bool Contains(const KEY& key) const;
    /// add new element
    void Add(const KeyValuePair<KEY, VALUE>& kvp);
    /// add new element
    void Add(KeyValuePair<KEY, VALUE>&& kvp);
    /// add new element
    void Add(const KEY& key, const VALUE& value);
    /// add new element, return false if element with key already existed
    bool AddUnique(const KeyValuePair<KEY, VALUE>& kvp);
    /// add new element with move-semantics, return false if element with key already existed
    bool AddUnique(KeyValuePair<KEY, VALUE>&& kvp);
    /// add new element, and check that it is unique
    bool AddUnique(const KEY& key, const VALUE& value);
    /// erase all elements matching key, does nothing if key not contained
    void Erase(const KEY& key);
    
    /// begin bulk-mode
    void BeginBulk();
    /// add element in bulk-mode (destroys sorting order)
    void AddBulk(const KeyValuePair<KEY, VALUE>& kvp);
    /// add element in bulk-mode (destroys sorting order)
    void AddBulk(KeyValuePair<KEY, VALUE>&& kvp);
    /// add element in bulk-mode (destroys sorting order)
    void AddBulk(const KEY& key, const VALUE& value);
    /// end bulk-mode (sorting happens here)
    void EndBulk();
    /// find the first duplicate element, or InvalidIndex if not found, this is O(N)!
    int FindDuplicate(int startIndex) const;
    /// find an element, returns index, or InvalidIndex
    int FindIndex(const KEY& key) const;
    /// erase element at index
    void EraseIndex(int index);
    /// get key at index
    const KEY& KeyAtIndex(int index) const;
    /// get value at index (read-only)
    const VALUE& ValueAtIndex(int index) const;
    /// get value at index (read/write)
    VALUE& ValueAtIndex(int index);
    
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
    void adjustCapacity(int newCapacity);
    /// grow to make room
    void grow();
    
    _priv::elementBuffer<KeyValuePair<KEY,VALUE>> buffer;
    int minGrow;
    int maxGrow;
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

template<class KEY, class VALUE>
Map<KEY, VALUE>::Map(std::initializer_list<KeyValuePair<KEY,VALUE>> rhs) :
minGrow(ORYOL_CONTAINER_DEFAULT_MIN_GROW),
maxGrow(ORYOL_CONTAINER_DEFAULT_MAX_GROW),
inBulkMode(false) {
	this->BeginBulk();
	for (const KeyValuePair<KEY,VALUE> &kvp : rhs) {
		this->AddBulk(kvp);
	}
	this->EndBulk();
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
Map<KEY, VALUE>::SetAllocStrategy(int minGrow_, int maxGrow_) {
    this->minGrow = minGrow_;
    this->maxGrow = maxGrow_;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int
Map<KEY, VALUE>::GetMinGrow() const {
    return this->minGrow;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int
Map<KEY, VALUE>::GetMaxGrow() const {
    return this->maxGrow;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int
Map<KEY, VALUE>::Size() const {
    return this->buffer.size();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
Map<KEY, VALUE>::Empty() const {
    return this->buffer.size() == 0;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int
Map<KEY, VALUE>::Capacity() const {
    return this->buffer.capacity();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
Map<KEY, VALUE>::Contains(const KEY& key) const {
    o_assert_dbg(!this->inBulkMode);
    return std::binary_search(this->buffer._begin(), this->buffer._end(), key);
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> VALUE&
Map<KEY, VALUE>::operator[](const KEY& key) {
    o_assert_dbg(!this->inBulkMode);
    o_assert_dbg(this->buffer.buf);
    auto kvp = std::lower_bound(this->buffer._begin(), this->buffer._end(), key);
    o_assert((kvp != this->buffer._end()) && (key == kvp->key));    // not found if this triggers
    return kvp->value;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const VALUE&
Map<KEY, VALUE>::operator[](const KEY& key) const {
    o_assert_dbg(!this->inBulkMode);
    o_assert_dbg(this->buffer.buf);
    auto kvp = std::lower_bound(this->buffer._begin(), this->buffer._end(), key);
    o_assert_dbg((kvp != this->buffer._end()) && (key == kvp->key));    // not found if this triggers
    return kvp->value;
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::Reserve(int numElements) {
    int newCapacity = this->buffer.size() + numElements;
    if (newCapacity > this->buffer.capacity()) {
        this->adjustCapacity(newCapacity);
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::Trim() {
    const int curSize = this->buffer.size();
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
Map<KEY, VALUE>::Add(const KeyValuePair<KEY, VALUE>& kvp) {
    o_assert_dbg(!this->inBulkMode);
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    auto ptr = std::lower_bound(this->buffer._begin(), this->buffer._end(), kvp.key);
    int index = int(ptr - this->buffer._begin());
    this->buffer.insert(index, kvp);
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::Add(KeyValuePair<KEY, VALUE>&& kvp) {
    o_assert_dbg(!this->inBulkMode);
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    auto ptr = std::lower_bound(this->buffer._begin(), this->buffer._end(), kvp.key);
    int index = int(ptr - this->buffer._begin());
    this->buffer.insert(index, std::move(kvp));
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::Add(const KEY& key, const VALUE& value) {
    this->Add(KeyValuePair<KEY, VALUE>(key, value));
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
Map<KEY, VALUE>::AddUnique(const KeyValuePair<KEY, VALUE>& kvp) {
    o_assert(!this->inBulkMode);
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    auto ptr = std::lower_bound(this->buffer._begin(), this->buffer._end(), kvp.key);
    if ((ptr != this->buffer._end()) && (ptr->key == kvp.key)) {
        return false;
    }
    else {
        int index = int(ptr - this->buffer._begin());
        this->buffer.insert(index, kvp);
        return true;
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
Map<KEY, VALUE>::AddUnique(KeyValuePair<KEY, VALUE>&& kvp) {
    o_assert(!this->inBulkMode);
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    auto ptr = std::lower_bound(this->buffer._begin(), this->buffer._end(), kvp.key);
    if ((ptr != this->buffer._end()) && (ptr->key == kvp.key)) {
        return false;
    }
    else {
        int index = int(ptr - this->buffer._begin());
        this->buffer.insert(index, std::move(kvp));
        return true;
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
Map<KEY, VALUE>::AddUnique(const KEY& key, const VALUE& value) {
    return this->AddUnique(KeyValuePair<KEY, VALUE>(key, value));
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::Erase(const KEY& key) {
    auto ptr = std::lower_bound(this->buffer._begin(), this->buffer._end(), key);
    if (ptr != this->buffer._end()) {
        const int index = int(ptr - this->buffer._begin());
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
Map<KEY, VALUE>::AddBulk(const KeyValuePair<KEY, VALUE>& kvp) {
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
Map<KEY, VALUE>::AddBulk(KeyValuePair<KEY, VALUE>&& kvp) {
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
Map<KEY, VALUE>::AddBulk(const KEY& key, const VALUE& value) {
    this->Insert(KeyValuePair<KEY, VALUE>(key, value));
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::EndBulk() {
    o_assert(this->inBulkMode);
    this->inBulkMode = false;
    std::sort(this->buffer._begin(), this->buffer._end());
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int
Map<KEY, VALUE>::FindDuplicate(int startIndex) const {
    o_assert(!this->inBulkMode);
    const int size = this->buffer.size();
    if (startIndex < size) {
        for (int index = startIndex; index < (size - 1); index++) {
            if (this->buffer[index].key == this->buffer[index + 1].key) {
                return index;
            }
        }
    }
    return InvalidIndex;
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> int
Map<KEY, VALUE>::FindIndex(const KEY& key) const {
    o_assert(!this->inBulkMode);
    auto ptr = std::lower_bound(this->buffer._begin(), this->buffer._end(), key);
    if ((ptr != this->buffer._end()) && (key == ptr->key)) {
        return int(ptr - this->buffer._begin());
    }
    else {
        return InvalidIndex;
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::EraseIndex(int index) {
    this->buffer.erase(index);
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const KEY&
Map<KEY, VALUE>::KeyAtIndex(int index) const {
    return this->buffer[index].key;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const VALUE&
Map<KEY, VALUE>::ValueAtIndex(int index) const {
    return this->buffer[index].value;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> VALUE&
Map<KEY, VALUE>::ValueAtIndex(int index) {
    return this->buffer[index].value;
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> KeyValuePair<KEY, VALUE>*
Map<KEY, VALUE>::begin() {
    return this->buffer._begin();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const KeyValuePair<KEY, VALUE>*
Map<KEY, VALUE>::begin() const {
    return this->buffer._begin();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> KeyValuePair<KEY, VALUE>*
Map<KEY, VALUE>::end() {
    return this->buffer._end();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const KeyValuePair<KEY, VALUE>*
Map<KEY, VALUE>::end() const {
    return this->buffer._end();
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
    o_assert_dbg(!rhs.inBulkMode);
    this->minGrow    = rhs.minGrow;
    this->maxGrow    = rhs.maxGrow;
    this->inBulkMode = rhs.inBulkMode;
    this->buffer  = std::move(rhs.buffer);
    // NOTE: don't reset minGrow/maxGrow, rhs is empty, but still a valid object!
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::adjustCapacity(int newCapacity) {
    // have a balanced front and back spare
    int frontSpare = (newCapacity - this->buffer.size()) >> 1;
    o_assert_dbg(frontSpare >= 0);
    this->buffer.alloc(newCapacity, frontSpare);
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
Map<KEY, VALUE>::grow() {
    const int curCapacity = this->buffer.capacity();
    int growBy = curCapacity >> 1;
    if (growBy < minGrow) {
        growBy = minGrow;
    }
    else if (growBy > maxGrow) {
        growBy = maxGrow;
    }
    o_assert_dbg(growBy > 0);
    int newCapacity = curCapacity + growBy;
    this->adjustCapacity(newCapacity);
}

} // namespace Oryol
