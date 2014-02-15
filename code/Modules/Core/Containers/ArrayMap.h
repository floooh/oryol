#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::ArrayMap
    @brief map class which stores value in a separate array
    
    An ArrayMap works like a Map, but stores the values in a separate
    array from keys. This is useful if:
    
    - the value type is big and/or has an expensive copy/move operation
    - the values must remain in the order they have been added
    - erasing is only fast when the order of values is not important, 
      in this case, use EraseSwap (see notes on erasing below)
    
    Iterating over an ArrayMap with begin(), end() will iterate over 
    the values in the order the values have been added.
    
    The operator[] will access elements by key (just as in a map). To 
    access an element by its value index, use ValueAtIndex()!
    
    Be careful about erasing elements, this can be slow:
    - EraseSwap() is best, this do a swap in the value array, but still
      needs to iterate over the keymap to find and replace the swapped-in index
    - Erase() and EraseIndex() need to do a sweep over the key map to
      fix-up indices, and are thus O(N)!!!
      
    @see Array, HashSet, Map, Set
*/
#include "Core/Config.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Map.h"

namespace Oryol {
namespace Core {

template<class KEY, class VALUE> class ArrayMap {
public:
    /// default constructor
    ArrayMap();
    /// copy constructor (truncates to actual size)
    ArrayMap(const ArrayMap& rhs);
    /// move constructor (same capacity and size)
    ArrayMap(ArrayMap&& rhs);
    /// copy-assignment operator (truncates to actual size)
    void operator=(const ArrayMap& rhs);
    /// move-assignment operator (same capacity and size)
    void operator=(ArrayMap&& rhs);
    
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
    /// get capacity of array
    int32 Capacity() const;
    
    /// test if an element exists
    bool Contains(const KEY& key) const;

    /// read/write access single element by key
    VALUE& operator[](const KEY& key);
    /// read-only access single element by key
    const VALUE& operator[](const KEY& key) const;
    
    /// increase capacity to hold at least numElements more elements
    void Reserve(int32 numElements);
    /// trim capacity to size (this involves a re-alloc)
    void Trim();
    /// clear the array (deletes elements, keeps capacity)
    void Clear();
    
    /// insert-copy new element
    void Insert(const KEY& key, const VALUE& value);
    /// insert-move element
    void Insert(KEY&& key, VALUE&& value);
    
    /// find value index
    int32 FindValueIndex(const KEY& key);
    /// read-access value at index
    const VALUE& ValueAtIndex(int32 valueIndex) const;
    /// read-write-access value at index
    VALUE& ValueAtIndex(int32 valueIndex);
    
    /// erase element by key, reasonably fast, but destroys value ordering
    void EraseSwap(const KEY& key);
    /// erase element by key, keep value order (involves moving values around)
    void Erase(const KEY& key);
    
    /// C++ conform begin, MAY RETURN nullptr!
    VALUE* begin();
    /// C++ conform begin, MAY RETURN nullptr!
    const VALUE* begin() const;
    /// C++ conform end,  MAY RETURN nullptr!
    VALUE* end();
    /// C++ conform end, MAY RETURN nullptr!
    VALUE* end() const;
    
private:
    Map<KEY, int32> indexMap;        // maps keys to indices into value array
    Array<VALUE> valueArray;
};

//------------------------------------------------------------------------------
template<class KEY, class VALUE>
ArrayMap<KEY, VALUE>::ArrayMap() {
    // empty
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE>
ArrayMap<KEY, VALUE>::ArrayMap(const ArrayMap& rhs) :
indexMap(rhs.indexMap),
valueArray(rhs.valueArray) {
    // empty
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE>
ArrayMap<KEY, VALUE>::ArrayMap(ArrayMap&& rhs) :
indexMap(std::move(rhs.indexMap)),
valueArray(std::move(rhs.valueArray)) {
    // empty
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
ArrayMap<KEY, VALUE>::operator=(const ArrayMap& rhs) {
    if (&rhs != this) {
        this->indexMap = rhs.indexMap;
        this->valueArray = rhs.valueArray;
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
ArrayMap<KEY, VALUE>::operator=(ArrayMap&& rhs) {
    if (&rhs != this) {
        this->indexMap = std::move(rhs.indexMap);
        this->valueArray = std::move(rhs.valueArray);
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
ArrayMap<KEY, VALUE>::SetAllocStrategy(int32 minGrow, int32 maxGrow) {
    this->indexMap.SetAllocStrategy(minGrow, maxGrow);
    this->valueArray.SetAllocStrategy(minGrow, maxGrow);
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int32
ArrayMap<KEY, VALUE>::GetMinGrow() const {
    return this->valueArray.GetMinGrow();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int32
ArrayMap<KEY, VALUE>::GetMaxGrow() const {
    return this->valueArray.GetMaxGrow();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int32
ArrayMap<KEY, VALUE>::Size() const {
    return this->valueArray.Size();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
ArrayMap<KEY, VALUE>::Empty() const {
    return this->valueArray.Empty();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> int32
ArrayMap<KEY, VALUE>::Capacity() const {
    return this->valueArray.Capacity();
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
ArrayMap<KEY, VALUE>::Contains(const KEY& key) const {
    return this->indexMap.Contains(key);
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> VALUE&
ArrayMap<KEY, VALUE>::operator[](const KEY& key) {
    return this->valueArray[this->indexMap[key]];
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const VALUE&
ArrayMap<KEY, VALUE>::operator[](const KEY& key) const {
    return this->valueArray[this->indexMap[key]];
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
ArrayMap<KEY, VALUE>::Reserve(int32 numElements) {
    this->indexMap.Reserve(numElements);
    this->valueArray.Reserve(numElements);
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
ArrayMap<KEY, VALUE>::Trim() {
    this->indexMap.Trim();
    this->valueArray.Trim();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
ArrayMap<KEY, VALUE>::Clear() {
    this->indexMap.Clear();
    this->valueArray.Clear();
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
ArrayMap<KEY, VALUE>::Insert(const KEY& key, const VALUE& value) {
    this->valueArray.AddBack(value);
    this->indexMap.Insert(key, this->valueArray.Size() - 1);
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
ArrayMap<KEY, VALUE>::Insert(KEY&& key, VALUE&& value) {
    this->valueArray.AddBack(std::move(value));
    this->indexMap.Insert(std::move(key), this->valueArray.Size() - 1);
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> int32
ArrayMap<KEY, VALUE>::FindValueIndex(const KEY& key) {
    return this->indexMap[key];
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const VALUE&
ArrayMap<KEY, VALUE>::ValueAtIndex(int32 valueIndex) const {
    return this->valueArray[valueIndex];
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> VALUE&
ArrayMap<KEY, VALUE>::ValueAtIndex(int32 valueIndex) {
    return this->valueArray[valueIndex];
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
ArrayMap<KEY, VALUE>::EraseSwap(const KEY& key) {
    const int32 mapIndex = this->indexMap.FindIndex(key);
    const int32 valueIndex = this->indexMap.ValueAtIndex(mapIndex);
    this->indexMap.EraseIndex(mapIndex);
    this->valueArray.EraseSwapBack(valueIndex);
    
    // fix-up indices
    const int32 swappedIndex = this->valueArray.Size();
    if (valueIndex != swappedIndex) {
        for (auto& elm : this->indexMap) {
            if (swappedIndex == elm.value) {
                elm.value = valueIndex;
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
ArrayMap<KEY, VALUE>::Erase(const KEY& key) {
    const int32 mapIndex = this->indexMap.FindIndex(key);
    const int32 valueIndex = this->indexMap.ValueAtIndex(mapIndex);
    this->indexMap.EraseIndex(mapIndex);
    this->valueArray.Erase(valueIndex);
    
    // fix up indices
    for (auto& elm : this->indexMap) {
        if (elm.value > valueIndex) {
            elm.value--;
        }
    }
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> VALUE*
ArrayMap<KEY, VALUE>::begin() {
    return this->valueArray.begin();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const VALUE*
ArrayMap<KEY, VALUE>::begin() const {
    return this->valueArray.begin();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> VALUE*
ArrayMap<KEY, VALUE>::end() {
    return this->valueArray.end();
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> VALUE*
ArrayMap<KEY, VALUE>::end() const {
    return this->valueArray.end();
}

} // namespace Core
} // namespace Oryol

