#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Array
    
    Dynamic array class (similar to std::vector) with room for growth at
    the end. Internally this is a double-ended array, e.g. erasing items
    at the front does not involve a move-operation, and erasing in the
    middle will move either from the front or the back, depending on what
    would involve less move ops.
 
    When working with pointers or references into the array keep in mind
    that operations which add new elements may reallocate the whole array
    at a different memory location!
    
    NOTE: An array *copy* operation will truncate the capacity to the
    number of elements, so that no spare room is in the array. A *move*
    will not truncate but keep the previous size and capacity.
    
    NOTE: An array growth operation will truncate any spare room
    at the front.
    
    For sorting, iterating and sorted insertion, use the standard 
    <algorithm> stuff!
*/
#include "Core/Containers/elementBuffer.h"

namespace Oryol {
namespace Core {

template<class TYPE> class Array {
public:
    /// default allocation strategy values
    static const int32 DefaultMinGrow = 16;
    static const int32 DefaultMaxGrow = (1<<16);

    /// default constructor
    Array();
    /// setup with allocation strategy
    Array(int32 minGrow, int32 maxGrow=DefaultMaxGrow);
    /// copy constructor (truncates to actual size)
    Array(const Array& rhs);
    /// move constructor (same capacity and size)
    Array(Array&& rhs);
    /// destructor
    ~Array();

    /// copy-assignment operator (truncates to actual size)
    void operator=(const Array& rhs);
    /// move-assignment operator (same capacity and size)
    void operator=(Array&& rhs);
    
    /// set allocation strategy (default is mode=BackEnded, minGrow=16, maxGrow=
    void SetAllocStrategy(int32 minGrow=DefaultMinGrow, int32 maxGrow=DefaultMaxGrow);
    /// get min grow value
    int32 GetMinGrow() const;
    /// get max grow value
    int32 GetMaxGrow() const;
    /// get number of elements in array
    int32 Size() const;
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

    /// reserve space for new capacity (reallocates and moves elements)
    void Reserve(int32 newCapacity);
    /// clear the array (deletes elements, keeps capacity)
    void Clear();
    
    /// copy-add element to back of array (only works on BackEnded or DoubleEnded arrays)
    void AddBack(const TYPE& elm);
    /// move-add element to back of array (only works on BackEnded or DoubleEnded arrays)
    void AddBack(TYPE&& elm);
    /// copy-insert element at index, keep array order (works on all array types)
    void Insert(int32 index, const TYPE& elm);
    /// move-insert element at index, keep array order (works on all array types)
    void Insert(int32 index, TYPE&& elm);
    /// emplace new element at back of array (only works on BackEnded or DoubleEnded arrays)
    template<class... ARGS> void EmplaceBack(ARGS&&... args);
    /// emplate new element at index (works on all array types)
    template<class... ARGS> void EmplaceInsert(int32 index, ARGS&&... args);
    
    /// erase element at index, keep element ordering
    void Erase(int32 index);
    /// erase element at index, swap-in front or back element (destroys element ordering)
    void EraseSwap(int32 index);
    
    /// read/write pointer to first element
    TYPE* Begin();
    /// read-only pointer to first element
    const TYPE* Begin() const;
    /// pointer to one-past-end last element
    const TYPE* End() const;
    
private:
    /// destroy array resources
    void destroy();
    /// copy from other array
    void copy(const Array& rhs);
    /// move from other array
    void move(Array&& rhs);
    
    elementBuffer<TYPE> buffer;
    int32 minGrow;
    int32 maxGrow;
};

//------------------------------------------------------------------------------
template<class TYPE>
Array<TYPE>::destroy() {
    this->minGrow = 0;
    this->maxGrow = 0;
    this->buffer.destroy();
}

//------------------------------------------------------------------------------
template<class TYPE>
Array<TYPE>::copy(const Array& rhs) {
    this->minGrow = rhs.minGrow;
    this->maxGrow = rhs.maxGrow;
    this->buffer = rhs.buffer;
}

//------------------------------------------------------------------------------
template<class TYPE>
Array<TYPE>::move(Array&& rhs) {
    this->minGrow   = rhs.minGrow;
    this->maxGrow   = rhs.maxGrow;
    this->buffer    = std::move(rhs.buffer);
    rhs.minGrow = 0;
    rhs.maxGrow = 0;
}

//------------------------------------------------------------------------------
template<class TYPE>
Array<TYPE>::Array() :
    minGrow(DefaultMinGrow),
    maxGrow(DefaultMaxGrow)
{
    // empty
}

//------------------------------------------------------------------------------
template<class TYPE>
Array<TYPE>::Array(AllocMode mode, int32 minG, int32 maxG) :
    minGrow(minG),
    maxGrow(maxG)
{
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
    this->move(rhs);
}

//------------------------------------------------------------------------------
template<class TYPE>
Array<TYPE>::~Array() {
    this->destroy();
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::operator=(const Array& rhs) {
    this->destroy();
    this->copy();
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::operator=(Array&& rhs) {
    this->destroy();
    this->move(rhs);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::SetAllocStrategy(int32 minG, int32 maxG) {
    this->minGrow = minG;
    this->maxGrow = maxG;
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
    return this->buffer[index]
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
template<class TYPE> TYPE&
Array<TYPE>::Back() {
    return this->buffer.back();
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Reserve(int32 newCapacity) {
    this->growTo(newCapacity);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Clear() {
    this->buffer.clear();
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::AddBack(const TYPE& elm) {
    if (!this->buffer.backSpare() == 0) {
        this->grow();
    }
    this->buffer.pushBack(elm);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::AddBack(TYPE&& elm) {
    if (!this->buffer.backSpare() == 0) {
        this->grow();
    }
    this->buffer.pushBack(std::move(elm));
}

//------------------------------------------------------------------------------
template<class TYPE> void
Array<TYPE>::Insert(int32 index, const TYPE& elm) {
    if (this->buffer.spare() == 0) {
        this->grow()
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
    this->buffer.emplaceBack(std::forward(args));
}

//------------------------------------------------------------------------------
template<class TYPE> template<class... ARGS> void
Array<TYPE>::EmplaceInsert(int32 index, ARGS&&... args) {
    if (this->buffer.spare() == 0) {
        this->grow();
    }
    this->buffer.emplaceInsert(index, std::forward(args));
}

} // namespace Core
} // namespace Oryol