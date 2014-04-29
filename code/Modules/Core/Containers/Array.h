#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Array
    @brief dynamic array class similar to std::vector
    
    Dynamic array class (similar to std::vector) with room for growth at
    the end. Internally this is a double-ended array, e.g. erasing items
    at the front does not involve a move-operation, and erasing in the
    middle will move either from the front or the back, depending on what
    would involve less move ops, or use EraseSwap() if the element
    ordering is not important. If there is no spare room at the front
    (this is the is always the case, unless an element has been
    removed from the front), insertion at the front will involve
    size() move operations.
 
    When working with pointers or references into the array keep in mind
    that operations which add new elements may reallocate the whole array
    at a different memory location!
    
    NOTE: An array *copy* operation will trim the capacity to the
    number of elements, so that no spare room is in the array. A *move*
    will not truncate but keep the previous size and capacity.
    
    NOTE: An array growth operation will truncate any spare room
    at the front.
    
    For sorting, iterating and sorted insertion, use the standard 
    algorithm stuff!
    
    @see ArrayMap, Map, Set, HashSet
*/
#include "Core/Config.h"
#include "Core/Containers/elementBuffer.h"
#include <initializer_list>

namespace Oryol {
namespace Core {

template<class TYPE> class Array {
public:
    /// default constructor
    Array();
    /// copy constructor (truncates to actual size)
    Array(const Array& rhs);
    /// move constructor (same capacity and size)
    Array(Array&& rhs);
    /// initialize from initializer list
    Array(std::initializer_list<TYPE> l);
    /// destructor
    ~Array();

    /// copy-assignment operator (truncates to actual size)
    void operator=(const Array& rhs);
    /// move-assignment operator (same capacity and size)
    void operator=(Array&& rhs);
    
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
    /// get number of free slots at back of array
    int32 Spare() const;
    
    /// read/write access single element
    TYPE& operator[](int32 index);
    /// read-only access single element
    const TYPE& operator[](int32 index) const;
    /// read/write access to first element
    TYPE& Front();
    /// read-only access to first element
    const TYPE& Front() const;
    /// read/write access to last element
    TYPE& Back();
    /// read-only access to first element
    const TYPE& Back() const;

    /// increase capacity to hold at least numElements more elements
    void Reserve(int32 numElements);
    /// trim capacity to size (this involves a re-alloc)
    void Trim();
    /// clear the array (deletes elements, keeps capacity)
    void Clear();
    
    /// copy-add element to back of array
    void AddBack(const TYPE& elm);
    /// move-add element to back of array
    void AddBack(TYPE&& elm);
    /// copy-insert element at index, keep array order
    void Insert(int32 index, const TYPE& elm);
    /// move-insert element at index, keep array order
    void Insert(int32 index, TYPE&& elm);
    /// emplace new element at back of array
    template<class... ARGS> void EmplaceBack(ARGS&&... args);
    
    /// erase element at index, keep element ordering
    void Erase(int32 index);
    /// erase element at index, swap-in front or back element (destroys element ordering)
    void EraseSwap(int32 index);
    /// erase element at index, always swap-in from back (destroys element ordering)
    void EraseSwapBack(int32 index);
    /// erase element at index, always swap-in from front (destroys element ordering)
    void EraseSwapFront(int32 index);
    
    /// find element index with slow linear search
    int32 FindIndexLinear(const TYPE& elm, int32 startIndex=0, int32 endIndex=InvalidIndex) const;
    
    /// C++ conform begin
    TYPE* begin();
    /// C++ conform begin
    const TYPE* begin() const;
    /// C++ conform end
    TYPE* end();
    /// C++ conform end
    const TYPE* end() const;
    
private:
    /// destroy array resources
    void destroy();
    /// copy from other array
    void copy(const Array& rhs);
    /// move from other array
    void move(Array&& rhs);
    /// reallocate with new capacity
    void adjustCapacity(int32 newCapacity);
    /// grow to make room
    void grow();
    
    elementBuffer<TYPE> buffer;
    int32 minGrow;
    int32 maxGrow;
};

//------------------------------------------------------------------------------
template<class TYPE>
Array<TYPE>::Array() :
minGrow(ORYOL_CONTAINER_DEFAULT_MIN_GROW),
maxGrow(ORYOL_CONTAINER_DEFAULT_MAX_GROW) {
    // empty
}

//------------------------------------------------------------------------------
template<class TYPE>
Array<TYPE>::Array(const Array& rhs) {
    this->copy(rhs);
}

//------------------------------------------------------------------------------
template<class TYPE>
Array<TYPE>::Array(Array&& rhs) {
    this->move(std::move(rhs));
}

//------------------------------------------------------------------------------
template<class TYPE>
Array<TYPE>::Array(std::initializer_list<TYPE> l) :
minGrow(ORYOL_CONTAINER_DEFAULT_MIN_GROW),
maxGrow(ORYOL_CONTAINER_DEFAULT_MAX_GROW) {
    this->Reserve(l.size());
    for (const auto& elm : l) {
        this->AddBack(elm);
    }
}

//------------------------------------------------------------------------------
template<class TYPE>
Array<TYPE>::~Array() {
    this->destroy();
};

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::operator=(const Array<TYPE>& rhs) {
    /// @todo: this should be optimized when rhs.size() < this->capacity()!
    if (&rhs != this) {
        this->destroy();
        this->copy(rhs);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::operator=(Array<TYPE>&& rhs) {
    /// @todo: this should be optimized when rhs.size() < this->capacity()!
    if (&rhs != this) {
        this->destroy();
        this->move(std::move(rhs));
    }
}
    
//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::SetAllocStrategy(int32 minGrow_, int32 maxGrow_) {
    this->minGrow = minGrow_;
    this->maxGrow = maxGrow_;
}

//------------------------------------------------------------------------------
template<class TYPE> int32
Array<TYPE>::GetMinGrow() const {
        return this->minGrow;
    }
    
//------------------------------------------------------------------------------
template<class TYPE> int32
Array<TYPE>::GetMaxGrow() const {
    return this->maxGrow;
}

//------------------------------------------------------------------------------
template<class TYPE> int32
Array<TYPE>::Size() const {
    return this->buffer.size();
}

//------------------------------------------------------------------------------
template<class TYPE> bool
Array<TYPE>::Empty() const {
    return this->buffer.elmStart == this->buffer.elmEnd;
}

//------------------------------------------------------------------------------
template<class TYPE> int32
Array<TYPE>::Capacity() const {
    return this->buffer.capacity();
}

//------------------------------------------------------------------------------
template<class TYPE> int32
Array<TYPE>::Spare() const {
    return this->buffer.backSpare();
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
Array<TYPE>::operator[](int32 index) {
    return this->buffer[index];
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
Array<TYPE>::operator[](int32 index) const {
    return this->buffer[index];
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
Array<TYPE>::Front() {
    return this->buffer.front();
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
Array<TYPE>::Front() const {
    return this->buffer.front();
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
Array<TYPE>::Back() {
    return this->buffer.back();
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
Array<TYPE>::Back() const {
    return this->buffer.back();
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Reserve(int32 numElements) {
    int32 newCapacity = this->buffer.size() + numElements;
    if (newCapacity > this->buffer.capacity()) {
        this->adjustCapacity(newCapacity);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Trim() {
    const int32 curSize = this->buffer.size();
    if (curSize < this->buffer.capacity()) {
        this->adjustCapacity(curSize);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Clear() {
    this->buffer.clear();
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::AddBack(const TYPE& elm) {
    if (this->buffer.backSpare() == 0) {
        this->grow();
    }
    this->buffer.pushBack(elm);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::AddBack(TYPE&& elm) {
    if (this->buffer.backSpare() == 0) {
        this->grow();
    }
    this->buffer.pushBack(std::move(elm));
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Insert(int32 index, const TYPE& elm) {
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    this->buffer.insert(index, elm);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Insert(int32 index, TYPE&& elm) {
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    this->buffer.insert(index, std::move(elm));
}

//------------------------------------------------------------------------------
template<class TYPE> template<class... ARGS> void
Array<TYPE>::EmplaceBack(ARGS&&... args) {
    if (this->buffer.backSpare() == 0) {
        this->grow();
    }
    this->buffer.emplaceBack(std::forward<ARGS>(args)...);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Erase(int32 index) {
    this->buffer.erase(index);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::EraseSwap(int32 index) {
    this->buffer.eraseSwap(index);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::EraseSwapBack(int32 index) {
    this->buffer.eraseSwapBack(index);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::EraseSwapFront(int32 index) {
    this->buffer.eraseSwapFront(index);
}
    
//------------------------------------------------------------------------------
template<class TYPE> int32
Array<TYPE>::FindIndexLinear(const TYPE& elm, int32 startIndex, int32 endIndex) const {
    const int32 size = this->buffer.size();
    if (size > 0) {
        o_assert(startIndex < size);
        if (InvalidIndex == endIndex) {
            endIndex = size;
        }
        else {
            o_assert(endIndex <= size);
        }
        o_assert(startIndex <= endIndex);
        for (int32 i = startIndex; i < endIndex; i++) {
            TYPE* ptr = this->buffer.elmStart + i;
            if (elm == *ptr) {
                return i;
            }
        }
    }
    // fallthrough: not found
    return InvalidIndex;
}
    
//------------------------------------------------------------------------------
template<class TYPE> TYPE*
Array<TYPE>::begin() {
    return this->buffer.elmStart;
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE*
Array<TYPE>::begin() const {
    return this->buffer.elmStart;
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
Array<TYPE>::end() {
    return this->buffer.elmEnd;
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE*
Array<TYPE>::end() const {
    return this->buffer.elmEnd;
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::destroy() {
    this->minGrow = 0;
    this->maxGrow = 0;
    this->buffer.destroy();
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::copy(const Array& rhs) {
    this->minGrow = rhs.minGrow;
    this->maxGrow = rhs.maxGrow;
    this->buffer = rhs.buffer;
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::move(Array&& rhs) {
    this->minGrow = rhs.minGrow;
    this->maxGrow = rhs.maxGrow;
    this->buffer  = std::move(rhs.buffer);
    // NOTE: don't reset minGrow/maxGrow, rhs is empty, but still a valid object!
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::adjustCapacity(int32 newCapacity) {
    this->buffer.alloc(newCapacity, 0);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::grow() {
    const int32 curCapacity = this->buffer.capacity();
    int growBy = curCapacity >> 1;
    if (growBy < minGrow) {
        growBy = minGrow;
    }
    else if (growBy > maxGrow) {
        growBy = maxGrow;
    }
    o_assert(growBy > 0);
    int32 newCapacity = curCapacity + growBy;
    this->adjustCapacity(newCapacity);
}

} // namespace Core
} // namespace Oryol