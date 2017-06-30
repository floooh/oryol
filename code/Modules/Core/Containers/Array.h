#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Array
    @ingroup Core
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
#include "Core/Containers/Slice.h"
#include <initializer_list>

namespace Oryol {

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
    void SetAllocStrategy(int minGrow_, int maxGrow_=ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    /// initialize the array to a fixed capacity (guarantees that no re-allocs happen)
    void SetFixedCapacity(int fixedCapacity);
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
    /// get number of free slots at back of array
    int Spare() const;
    
    /// read/write access an existing element
    TYPE& operator[](int index);
    /// read-only access to existing element
    const TYPE& operator[](int index) const;
    /// read/write access to first element (must exists)
    TYPE& Front();
    /// read-only access to first element (must exist)
    const TYPE& Front() const;
    /// read/write access to last element (must exist)
    TYPE& Back();
    /// read-only access to last element (must exist)
    const TYPE& Back() const;
    /// get a slice into the array (beware of iterator-invalidation!)
    Slice<TYPE> MakeSlice(int offset=0, int numItems=EndOfRange);

    /// increase capacity to hold at least numElements more elements
    void Reserve(int numElements);
    /// trim capacity to size (this involves a re-alloc)
    void Trim();
    /// clear the array (deletes elements, keeps capacity)
    void Clear();
    
    /// copy-add element to back of array
    TYPE& Add(const TYPE& elm);
    /// move-add element to back of array
    TYPE& Add(TYPE&& elm);
    /// construct-add new element at back of array
    template<class... ARGS> TYPE& Add(ARGS&&... args);
    /// copy-insert element at index, keep array order
    void Insert(int index, const TYPE& elm);
    /// move-insert element at index, keep array order
    void Insert(int index, TYPE&& elm);

    /// pop the last element
    TYPE PopBack();
    /// pop the first element
    TYPE PopFront();
    /// erase element at index, keep element order
    void Erase(int index);
    /// erase element at index, swap-in front or back element (destroys element ordering)
    void EraseSwap(int index);
    /// erase element at index, always swap-in from back (destroys element ordering)
    void EraseSwapBack(int index);
    /// erase element at index, always swap-in from front (destroys element ordering)
    void EraseSwapFront(int index);
    /// erase a range of elements, keep element order
    void EraseRange(int index, int num);
    
    /// find element index with slow linear search, return InvalidIndex if not found
    int FindIndexLinear(const TYPE& elm, int startIndex=0, int endIndex=InvalidIndex) const;
    
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
    void adjustCapacity(int newCapacity);
    /// grow to make room
    void grow();
    
    _priv::elementBuffer<TYPE> buffer;
    int minGrow;
    int maxGrow;
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
    this->Reserve(int(l.size()));
    for (const auto& elm : l) {
        this->Add(elm);
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
Array<TYPE>::SetAllocStrategy(int minGrow_, int maxGrow_) {
    this->minGrow = minGrow_;
    this->maxGrow = maxGrow_;
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::SetFixedCapacity(int fixedCapacity) {
    this->minGrow = 0;
    this->maxGrow = 0;
    if (fixedCapacity > this->buffer.capacity()) {
        this->adjustCapacity(fixedCapacity);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> int
Array<TYPE>::GetMinGrow() const {
        return this->minGrow;
    }
    
//------------------------------------------------------------------------------
template<class TYPE> int
Array<TYPE>::GetMaxGrow() const {
    return this->maxGrow;
}

//------------------------------------------------------------------------------
template<class TYPE> int
Array<TYPE>::Size() const {
    return this->buffer.size();
}

//------------------------------------------------------------------------------
template<class TYPE> bool
Array<TYPE>::Empty() const {
    return this->buffer.size() == 0;
}

//------------------------------------------------------------------------------
template<class TYPE> int
Array<TYPE>::Capacity() const {
    return this->buffer.capacity();
}

//------------------------------------------------------------------------------
template<class TYPE> int
Array<TYPE>::Spare() const {
    return this->buffer.backSpare();
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
Array<TYPE>::operator[](int index) {
    return this->buffer[index];
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
Array<TYPE>::operator[](int index) const {
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
template<class TYPE> Slice<TYPE>
Array<TYPE>::MakeSlice(int offset, int numItems) {
    if (numItems == EndOfRange) {
        numItems = this->buffer.size() - offset;
    }
    return Slice<TYPE>(this->buffer.buf, this->buffer.size(), offset, numItems);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Reserve(int numElements) {
    int newCapacity = this->buffer.size() + numElements;
    if (newCapacity > this->buffer.capacity()) {
        this->adjustCapacity(newCapacity);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Trim() {
    const int curSize = this->buffer.size();
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
template<class TYPE> TYPE&
Array<TYPE>::Add(const TYPE& elm) {
    if (this->buffer.backSpare() == 0) {
        this->grow();
    }
    this->buffer.pushBack(elm);
    return this->buffer.back();
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
Array<TYPE>::Add(TYPE&& elm) {
    if (this->buffer.backSpare() == 0) {
        this->grow();
    }
    this->buffer.pushBack(std::move(elm));
    return this->buffer.back();
}
    
//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Insert(int index, const TYPE& elm) {
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    this->buffer.insert(index, elm);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Insert(int index, TYPE&& elm) {
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    this->buffer.insert(index, std::move(elm));

}

//------------------------------------------------------------------------------
template<class TYPE> template<class... ARGS> TYPE&
Array<TYPE>::Add(ARGS&&... args) {
    if (this->buffer.backSpare() == 0) {
        this->grow();
    }
    this->buffer.emplaceBack(std::forward<ARGS>(args)...);
    return this->buffer.back();
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE
Array<TYPE>::PopBack() {
    return this->buffer.popBack();
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE
Array<TYPE>::PopFront() {
    return this->buffer.popFront();
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Erase(int index) {
    this->buffer.erase(index);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::EraseSwap(int index) {
    this->buffer.eraseSwap(index);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::EraseSwapBack(int index) {
    this->buffer.eraseSwapBack(index);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::EraseSwapFront(int index) {
    this->buffer.eraseSwapFront(index);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::EraseRange(int index, int num) {
    this->buffer.eraseRange(index, num);
}

//------------------------------------------------------------------------------
template<class TYPE> int
Array<TYPE>::FindIndexLinear(const TYPE& elm, int startIndex, int endIndex) const {
    const int size = this->buffer.size();
    if (size > 0) {
        o_assert_dbg(startIndex < size);
        o_assert_dbg(this->buffer.buf);
        if (InvalidIndex == endIndex) {
            endIndex = size;
        }
        else {
            o_assert_dbg(endIndex <= size);
        }
        o_assert_dbg(startIndex <= endIndex);
        for (int i = startIndex; i < endIndex; i++) {
            TYPE* ptr = &(this->buffer.buf[this->buffer.start + i]);
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
    return this->buffer._begin();
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE*
Array<TYPE>::begin() const {
    return this->buffer._begin();
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
Array<TYPE>::end() {
    return this->buffer._end();
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE*
Array<TYPE>::end() const {
    return this->buffer._end();
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
Array<TYPE>::adjustCapacity(int newCapacity) {
    this->buffer.alloc(newCapacity, 0);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::grow() {
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
