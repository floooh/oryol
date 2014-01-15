#pragma once
//------------------------------------------------------------------------------
/**
    @class Core::elementBuffer
    
    DO NOT USE THIS CLASS DIRECTLY, INSTEAD USE THE "PUBLIC" CONTAINERS
    (starting with a capital letter).
 
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
    /// get overal spare (frontSpare + backSpare)
    int32 spare() const;
    /// get number of valid elements
    int32 size() const;
    /// get overall capacity
    int32 capacity() const;
    
    /// access element by index
    TYPE& operator[](int32 index);
    /// read-access element by index
    const TYPE& operator[](int32 index) const;
    /// get front element (r/w)
    TYPE& front();
    /// get front element (r/o)
    const TYPE& front() const;
    /// get back element (r/w)
    TYPE& back();
    /// get back element (r/o)
    const TYPE& back() const;
    
    /// allocate, grow or shrink the elementBuffer
    void alloc(int32 capacity, int32 frontSpare);
    /// destroy all
    void destroy();
    /// destroy element at pointer
    static void destroyElement(TYPE* ptr);
    /// clear content
    void clear();
    
    /// test if to pointer is within [from, from+num]
    static bool overlaps(const TYPE* from, const TYPE* to, int32 num);
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
    
    /// prepare insertion by making room and returning pointer to insert to
    TYPE* prepareInsert(int32 index);
    /// copy-insert element at position, keep other elements in their previous order
    void insert(int32 index, const TYPE& elm);
    /// move-insert element at position, keep other elements in their previous order
    void insert(int32 index, TYPE&& elm);

    /// erase element at index, keeps array ordering
    void erase(int32 index);
    /// erase element at index, swap in element from back or front, messes up ordering
    void eraseSwap(int32 index);
    
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
    this->alloc(rhs.size(), 0);
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
template<class TYPE>
elementBuffer<TYPE>::~elementBuffer<TYPE>() {
    this->destroy();
}
    
//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::operator=(const elementBuffer<TYPE>& rhs) {
    this->destroy();
    this->alloc(rhs.size(), 0);
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
elementBuffer<TYPE>::spare() const {
    return this->capacity() - this->size();
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
    o_assert((index >= 0) && (index < this->size()));
    return this->elmStart[index];
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
elementBuffer<TYPE>::operator[](int32 index) const {
    o_assert((index >= 0) && (index < this->size()));
    return this->elmStart[index];
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
elementBuffer<TYPE>::front() {
    o_assert(nullptr != this->elmStart);
    return *this->elmStart;
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
elementBuffer<TYPE>::front() const {
    o_assert(nullptr != this->elmStart);
    return *this->elmStart;
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
elementBuffer<TYPE>::back() {
    o_assert(nullptr != this->elmEnd);
    return *(this->elmEnd - 1);
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
elementBuffer<TYPE>::back() const {
    o_assert(nullptr != this->elmEnd);
    return *(this->elmEnd - 1);
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::alloc(int32 newCapacity, int32 newFrontSpare) {
    o_assert(this->capacity() != newCapacity);

    const int32 curSize = this->size();
    o_assert((newFrontSpare + curSize) <= newCapacity);

    // allocate new buffer
    const int32 newBufSize = newCapacity * sizeof(TYPE);
    TYPE* newBuffer = (TYPE*) Memory::Alloc(newBufSize);
    TYPE* newElmStart = newBuffer + newFrontSpare;
    
    // need to move any elements?
    if (curSize > 0) {
        // move elements over to new buffer
        move(this->elmStart, newElmStart, curSize);
    }
    
    // need to free old buffer?
    if (nullptr != this->bufStart) {
        Memory::Free(this->bufStart);
    }
    
    // replace pointers
    this->bufStart = newBuffer;
    this->bufEnd   = newBuffer + newCapacity;
    this->elmStart = newElmStart;
    this->elmEnd   = newElmStart + curSize;
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
template<class TYPE> bool
elementBuffer<TYPE>::overlaps(const TYPE* from, const TYPE* to, int32 num) {
    return (to >= from) && (to < (from + num));
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::copy(const TYPE* __restrict__ from, TYPE* __restrict__ to, int32 num) {
    o_assert(!overlaps(from, to, num));
    for (int i = 0; i < num; i++) {
        to[i] = from[i];
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::move(TYPE* __restrict__ from, TYPE* __restrict__ to, int32 num) {
    o_assert((from != nullptr) && (to != nullptr));
    if (from != to) {
        if (overlaps(from, to, num)) {
            // to is within [from, from+num], must do a backward move
            for (int i = num - 1; i >= 0; i--) {
                to[i] = std::move(from[i]);
            }
        }
        else {
            // do a normal forward move
            for (int i = 0; i < num; i++) {
                to[i] = std::move(from[i]);
            }
        }
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

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
elementBuffer<TYPE>::prepareInsert(int32 index) {
    const int32 size = this->size();
    o_assert((index >= 0) && (index <= size) && (0 != this->elmStart));
    
    // must be room either at start or end!
    o_assert((this->elmStart > this->bufStart) || (this->elmEnd < this->bufEnd));
    
    if (index == size) {
        // special case insert at end of array
        if (this->elmEnd < this->bufEnd) {
            // still room to spare at end
            return this->elmEnd++;
        }
        else if (this->elmStart > this->bufStart) {
            // make room by moving towards front (this should always be faster then reallocating)
            move(this->elmStart, this->elmStart - 1, size);
            this->elmStart--;
            return this->elmEnd - 1;
        }
        // can't happen
    }
    else if (0 == index) {
        // special case insert at front
        if (this->elmStart > this->bufStart) {
            // still room at front, don't need to move
            return --this->elmStart;
        }
        else if (this->elmEnd < this->bufEnd) {
            // no room at front, must make room by moving towards end
            move(this->elmStart, this->elmStart + 1, size);
            this->elmEnd++;
            return this->elmStart;
        }
        // can't happen
    }
    else {
        // a "normal" insert, decide whether to move forward or backward
        TYPE* ptr = this->elmStart + index;
        if (index < (size>>1)) {
            // prefer move toward front
            if (this->elmStart > this->bufStart) {
                // move towards front
                move(this->elmStart, this->elmStart - 1, index);
                this->elmStart--;
                ptr--;
            }
            else {
                // must move towards end
                o_assert(this->elmEnd < this->bufEnd);
                move(ptr, ptr + 1, size - index);
                this->elmEnd++;
            }
        }
        else {
            // prefer move toward end
            if (this->elmEnd < this->bufEnd) {
                // move towards back
                move(ptr, ptr + 1, size - index);
                this->elmEnd++;
            }
            else {
                // must move towards front
                o_assert(this->elmStart > this->bufStart);
                move(this->elmStart, this->elmStart - 1, index);
                this->elmStart--;
                ptr--;
            }
        }
        return ptr;
    }
    o_error("Can't happen.\n");
    return nullptr;
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::insert(int32 index, const TYPE& elm) {
    TYPE* ptr = this->prepareInsert(index);
    *ptr = elm;
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::insert(int32 index, TYPE&& elm) {
    TYPE* ptr = this->prepareInsert(index);
    *ptr = std::move(elm);
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::erase(int32 index) {
    const int32 size = this->size();
    o_assert((index >= 0) && (index < size) && (0 != this->elmStart));
    
    // call destructor on element
    TYPE* ptr = this->elmStart + index;
    ptr->~TYPE();
    
    if (0 == index) {
        // special case: first element
        this->elmStart++;
    }
    else if (index == (size - 1)) {
        // special case: last element
        this->elmEnd--;
    }
    else {
        // move the smaller amount of elements to fill the gap
        if (index < (size>>1)) {
            // move front elements
            move(this->elmStart, this->elmStart + 1, index);
            this->elmStart++;
        }
        else {
            // move back elements
            move(ptr + 1, ptr, size - index - 1);
            this->elmEnd--;
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::eraseSwap(int32 index) {
    const int32 size = this->size();
    o_assert((index >= 0) && (index < size) && (0 != this->elmStart));
    
    // call destructor on element
    TYPE* ptr = this->elmStart + index;
    ptr->~TYPE();
    
    if (0 == index) {
        // special case: first element
        this->elmStart++;
    }
    else if (index == (size - 1)) {
        // special case: last element
        this->elmEnd--;
    }
    else {
        // either swap in the first or last element (keep frontSpare and backSpare balanced)
        if (this->frontSpare() > this->backSpare()) {
            // swap-in element from back
            *ptr = std::move(*--this->elmEnd);
        }
        else {
            // swap-in element from front
            *ptr = std::move(*this->elmStart++);
        }
    }
}

} // namespace Core
} // namespace Oryol
//------------------------------------------------------------------------------
