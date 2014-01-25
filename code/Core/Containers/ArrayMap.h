#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::ArrayMap
    
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
*/
#include "Core/Config.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Map.h"

namespace Oryol {
namespace Core {

template<class KEY, class VALUE> class ArrayMap {
public:
    /// default constructor
    ArrayMap() {
    };
    /// construct with allocation strategy
    ArrayMap(int32 minGrow, int32 maxGrow=ORYOL_CONTAINER_DEFAULT_MAX_GROW) :
        indexMap(minGrow, maxGrow),
        valueArray(minGrow, maxGrow) {
    };
    /// copy constructor (truncates to actual size)
    ArrayMap(const ArrayMap& rhs) :
        indexMap(rhs.indexMap),
        valueArray(rhs.valueArray) {
    };
    /// move constructor (same capacity and size)
    ArrayMap(ArrayMap&& rhs) :
        indexMap(std::move(rhs.indexMap)),
        valueArray(std::move(rhs.valueArray)) {
    };
    /// copy-assignment operator (truncates to actual size)
    void operator=(const ArrayMap& rhs) {
        if (&rhs != this) {
            this->indexMap = rhs.indexMap;
            this->valueArray = rhs.valueArray;
        }
    };
    /// move-assignment operator (same capacity and size)
    void operator=(ArrayMap&& rhs) {
        if (&rhs != this) {
            this->indexMap = std::move(rhs.indexMap);
            this->valueArray = std::move(rhs.valueArray);
        }
    };

    /// set allocation strategy
    void SetAllocStrategy(int32 minGrow, int32 maxGrow=ORYOL_CONTAINER_DEFAULT_MAX_GROW) {
        this->indexMap.SetAllocStrategy(minGrow, maxGrow);
        this->valueArray.SetAllocStrategy(minGrow, maxGrow);
    };
    /// get min grow value
    int32 GetMinGrow() const {
        return this->valueArray.GetMinGrow();
    };
    /// get max grow value
    int32 GetMaxGrow() const {
        return this->valueArray.GetMaxGrow();
    };
    /// get number of elements in array
    int32 Size() const {
        return this->valueArray.Size();
    };
    /// return true if empty
    bool Empty() const {
        return this->valueArray.Empty();
    };
    /// get capacity of array
    int32 Capacity() const {
        return this->valueArray.Capacity();
    };
    
    /// test if an element exists
    bool Contains(const KEY& key) const {
        return this->indexMap.Contains(key);
    };

    /// read/write access single element by key
    VALUE& operator[](const KEY& key) {
        return this->valueArray[this->indexMap[key]];
    };
    /// read-only access single element by key
    const VALUE& operator[](const KEY& key) const {
        return this->valueArray[this->indexMap[key]];
    };
    
    /// increase capacity to hold at least numElements more elements
    void Reserve(int32 numElements) {
        this->indexMap.Reserve(numElements);
        this->valueArray.Reserve(numElements);
    };
    /// trim capacity to size (this involves a re-alloc)
    void Trim() {
        this->indexMap.Trim();
        this->valueArray.Trim();
    };
    /// clear the array (deletes elements, keeps capacity)
    void Clear() {
        this->indexMap.Clear();
        this->valueArray.Clear();
    };
    
    /// insert-copy new element
    void Insert(const KEY& key, const VALUE& value) {
        this->valueArray.AddBack(value);
        this->indexMap.Insert(key, this->valueArray.Size() - 1);
    };
    /// insert-move element
    void Insert(KEY&& key, VALUE&& value) {
        this->valueArray.AddBack(std::move(value));
        this->indexMap.Insert(std::move(key), this->valueArray.Size() - 1);
    };
    
    /// find value index
    int32 FindValueIndex(const KEY& key) {
        return this->indexMap[key];
    };
    /// read-access value at index
    const VALUE& ValueAtIndex(int32 valueIndex) const {
        return this->valueArray[valueIndex];
    };
    /// read-write-access value at index
    VALUE& ValueAtIndex(int32 valueIndex) {
        return this->valueArray[valueIndex];
    };
    
    /// erase element by key, reasonably fast, but destroys value ordering
    void EraseSwap(const KEY& key) {
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
    };
    /// erase element by key, keep value order (involves moving values around)
    void Erase(const KEY& key) {
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
    };
    
    /// C++ conform begin, MAY RETURN nullptr!
    VALUE* begin() {
        return this->valueArray.begin();
    };
    /// C++ conform begin, MAY RETURN nullptr!
    const VALUE* begin() const {
        return this->valueArray.begin();
    };
    /// C++ conform end,  MAY RETURN nullptr!
    VALUE* end() {
        return this->valueArray.end();
    };
    /// C++ conform end, MAY RETURN nullptr!
    VALUE* end() const {
        return this->valueArray.end();
    };
    
private:
    Map<KEY, int32> indexMap;        // maps keys to indices into value array
    Array<VALUE> valueArray;
};
    
} // namespace Core
} // namespace Oryol

