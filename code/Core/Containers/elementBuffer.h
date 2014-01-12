#pragma once
//------------------------------------------------------------------------------
/**
    @class Core::elementBuffer
    
    Low-level dynamic memory buffer that is used as base for most
    higher-level Oryol container classes. Internally the buffer
    and range of valid elements is managed through 4 pointers:
    
    bufStart - pointer to start of allocated memory
    bufEnd   - pointer to one-past-end of allocated memory
    elmStart - pointer to first valid element
    elmEnd   - pointer to one-past-end of valid elements
 
    |----|----|----|----|XXXX|XXXX|XXXX|XXXX|----|----|----|
    bufStart            elmStart             elmEnd         bufEnd
    
    '----' - empty memory slot
    'XXXX' - valid element
    
    FIXME: currently a copy from another elementBuffer will
    allocate the same capacity as the source buffer. Would it be
    better to truncate to the actual size? This doesn't make sense
    for a move operation though.
 
*/
#include "Core/Types.h"
#include "Core/Macros.h"
#include "Core/Memory/Memory.h"

//------------------------------------------------------------------------------
namespace Oryol {
namespace Core {
template<class TYPE> class elementBuffer {
public:
    /// default constructor
    elementBuffer();
    /// copy constructor
    elementBuffer(const elementBuffer& rhs);
    /// move constructor
    elementBuffer(elementBuffer&& rhs);
    /// destructor
    ~elementBuffer();
    
    /// copy-assignment operator
    void operator=(const elementBuffer& rhs);
    /// move-assignment operator
    void operator=(elementBuffer&& rhs);
    
    /// get number of free slots at front
    int32 frontSpare() const;
    /// get number of free slots at back
    int32 backSpare() const;
    /// get number of valid elements
    int32 size() const;
    /// get overall capacity
    int32 capacity() const;
    
    /// access element by index
    TYPE& operator[](int32 index);
    /// read-access element by index
    const TYPE& operator[](int32 index) const;
    
    /// first-time allocation
    void allocate(int32 capacity, int32 frontSpare);
    /// re-allocation
    void reallocate(int32 newCapacity, int32 frontSpare);
    /// destroy all
    void destroy();
    /// destroy element at pointer
    static void destroyElement(TYPE* ptr);
    /// clear content
    void clear();

    /// copy element range
    static void copy(const TYPE* __restrict__ from, TYPE* __restrict__ to, int32 num);
    /// move element range
    static void move(TYPE* __restrict__ from, TYPE* __restrict__ to, int32 num);
    
    /// push element at back (must have room)
    void pushBack(const TYPE& elm);
    /// move element to back
    void pushBack(TYPE&& elm);
    /// emplace element at back (must have room)
    template<class... ARGS> void emplaceBack(ARGS&&... args);

    /// push element at front (must have room)
    void pushFront(const TYPE& elm);
    /// move element at front
    void pushFront(TYPE&& elm);
    /// emplace element at front
    template<class... ARGS> void emplaceFront(ARGS&&... args);
    
    
protected:
    TYPE* bufStart;     // start of allocated buffer
    TYPE* bufEnd;       // end of allocated buffer
    TYPE* elmStart;     // start of valid elements
    TYPE* elmEnd;       // end of valid elements (one-past-last)
};

//------------------------------------------------------------------------------
template<class TYPE>
elementBuffer<TYPE>::elementBuffer() :
    bufStart(nullptr),
    bufEnd(nullptr),
    elmStart(nullptr),
    elmEnd(nullptr)
{
    // empty
}

//------------------------------------------------------------------------------
template<class TYPE>
elementBuffer<TYPE>::elementBuffer(const elementBuffer& rhs) :
    bufStart(nullptr),
    bufEnd(nullptr),
    elmStart(nullptr),
    elmEnd(nullptr)
{
    this->allocate(rhs.capacity(), rhs.frontSpare());
    copy(rhs.elmStart, this->elmStart, rhs.size());
    this->elmEnd = this->elmStart + rhs.size();
}

//------------------------------------------------------------------------------
template<class TYPE>
elementBuffer<TYPE>::elementBuffer(elementBuffer&& rhs) :
    bufStart(rhs.bufStart),
    bufEnd(rhs.bufEnd),
    elmStart(rhs.elmStart),
    elmEnd(rhs.elmEnd)
{
    rhs.bufStart = nullptr;
    rhs.bufEnd = nullptr;
    rhs.elmStart = nullptr;
    rhs.elmEnd = nullptr;
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::operator=(const elementBuffer<TYPE>& rhs) {
    this->destroy();
    this->allocate(rhs.capacity(), rhs.frontSpare());
    copy(rhs.elmStart, this->elmStart, rhs.size());
    this->elmEnd = this->elmStart + rhs.size();
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::operator=(elementBuffer<TYPE>&& rhs) {
    this->bufStart = rhs.bufStart;
    this->bufEnd   = rhs.bufEnd;
    this->elmStart = rhs.elmStart;
    this->elmEnd   = rhs.elmEnd;
    rhs.bufStart = 0;
    rhs.bufEnd   = 0;
    rhs.elmStart = 0;
    rhs.elmEnd   = 0;
}

//------------------------------------------------------------------------------
template<class TYPE>
elementBuffer<TYPE>::~elementBuffer<TYPE>() {
    this->destroy();
}

//------------------------------------------------------------------------------
template<class TYPE> int32
elementBuffer<TYPE>::frontSpare() const {
    return intptr(this->elmStart - this->bufStart);
}

//------------------------------------------------------------------------------
template<class TYPE> int32
elementBuffer<TYPE>::backSpare() const {
    return intptr(this->bufEnd - this->elmEnd);
}

//------------------------------------------------------------------------------
template<class TYPE> int32
elementBuffer<TYPE>::size() const {
    return intptr(this->elmEnd - this->elmStart);
}

//------------------------------------------------------------------------------
template<class TYPE> int32
elementBuffer<TYPE>::capacity() const {
    return intptr(this->bufEnd - this->bufStart);
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
elementBuffer<TYPE>::operator[](int32 index) {
    return this->elmStart[index];
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
elementBuffer<TYPE>::operator[](int32 index) const {
    return this->elmStart[index];
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::allocate(int32 capacity, int32 frontSpare) {
    o_assert(nullptr == this->bufStart);
    o_assert(frontSpare < capacity);
    
    const int32 bufSize = capacity * sizeof(TYPE);
    this->bufStart = (TYPE*) Memory::Alloc(bufSize);
    this->bufEnd   = this->bufStart + capacity;
    this->elmStart = this->bufStart + frontSpare;
    this->elmEnd   = this->elmStart;
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::reallocate(int32 newCapacity, int32 newFrontSpare) {
    o_assert(nullptr != this->bufStart);
    o_assert(newCapacity > this->capacity());
    o_assert((newFrontSpare + this->size()) < newCapacity);
    const int32 curSize = this->size();
    
    // allocate new buffer
    const int32 newBufSize = newCapacity * sizeof(TYPE);
    TYPE* newBuffer = (TYPE*) Memory::Alloc(newBufSize);
    
    // move elements over to new buffer
    TYPE* newElmStart = newBuffer + newFrontSpare;
    move(this->elmStart, newElmStart, curSize);
    
    // free old buffer and replace pointers
    Memory::Free(this->bufStart);
    this->bufStart = newBuffer;
    this->bufEnd   = this->bufStart + newCapacity;
    this->elmStart = newElmStart;
    this->elmEnd   = this->elmStart + curSize;
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::destroy() {
    // destroy elements
    if (nullptr != this->elmStart) {
        for (TYPE* ptr = this->elmStart; ptr < this->elmEnd; ptr++) {
            ptr->~TYPE();
        }
    }
    
    // free buffer
    if (nullptr != this->bufStart) {
        Memory::Free(this->bufStart);
    }
    
    // clear all pointers
    this->bufStart = nullptr;
    this->bufEnd   = nullptr;
    this->elmStart = nullptr;
    this->elmEnd   = nullptr;
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::destroyElement(TYPE* elm) {
    elm->~TYPE();
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::clear() {
    o_assert(nullptr != this->elmStart);
    for (TYPE* ptr = this->elmStart; ptr < this->elmEnd; ptr++) {
        ptr->~TYPE();
    }
    this->elmEnd = this->elmStart;
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::copy(const TYPE* __restrict__ from, TYPE* __restrict__ to, int32 num) {
    for (int i = 0; i < num; i++) {
        to[i] = from[i];
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::move(TYPE* __restrict__ from, TYPE* __restrict__ to, int32 num) {
    o_assert((from != nullptr) && (to != nullptr) && (from != to));
    for (int i = 0; i < num; i++) {
        to[i] = std::move(from[i]);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushBack(const TYPE& elm) {
    o_assert((nullptr != this->elmEnd) && (this->elmEnd < this->bufEnd));
    *this->elmEnd++ = elm;
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushBack(TYPE&& elm) {
    o_assert((nullptr != this->elmEnd) && (this->elmEnd < this->bufEnd));
    *this->elmEnd++ = std::move(elm);
}

//------------------------------------------------------------------------------
template<class TYPE> template<class... ARGS> void
elementBuffer<TYPE>::emplaceBack(ARGS&&... args) {
    o_assert((nullptr != this->elmEnd) && (this->elmEnd < this->bufEnd));
    new(this->elmEnd++) TYPE(std::forward<ARGS>(args)...);
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushFront(const TYPE& elm) {
    o_assert((nullptr != this->elmStart) && (this->elmStart > this->bufStart));
    *--this->elmStart = elm;
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushFront(TYPE&& elm) {
    o_assert((nullptr != this->elmStart) && (this->elmStart > this->bufStart));
    *--this->elmStart = std::move(elm);
}

//------------------------------------------------------------------------------
template<class TYPE> template<class... ARGS> void
elementBuffer<TYPE>::emplaceFront(ARGS&&... args) {
    o_assert((nullptr != this->elmStart) && (this->elmStart > this->bufStart));
    new(--this->elmStart) TYPE(std::forward<ARGS>(args)...);
}

} // namespace Core
} // namespace Oryol
//------------------------------------------------------------------------------
