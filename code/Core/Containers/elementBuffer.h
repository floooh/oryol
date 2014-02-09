#pragma once
//------------------------------------------------------------------------------
/*
    private class, do not use
 
    Low-level dynamic memory buffer that is used as base for most
    higher-level Oryol container classes. Internally the buffer
    and range of valid elements is managed through 4 pointers:
    
    bufStart - pointer to start of allocated memory
    bufEnd   - pointer to one-past-end of allocated memory
    elmStart - pointer to first valid element
    elmEnd   - pointer to one-past-end of valid elements
 
    |----|----|----|----|XXXX|XXXX|XXXX|XXXX|----|----|----|
    bufStart            elmStart             elmEnd         bufEnd
    
    '----' - empty memory slot (guaranteed to be destructed)
    'XXXX' - valid element (guaranteed to be constructed)
*/
#include <new>
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
    /// copy-construct element range
    static void copyConstruct(const TYPE* from, TYPE* to, int32 num);
    /// copy-assign element range UNTESTED
    static void copyAssign(const TYPE* from, TYPE* to, int32 num);
    
    /// push element at back (backSpare must be > 0!)
    void pushBack(const TYPE& elm);
    /// move element to back (backSpare must be > 0!)
    void pushBack(TYPE&& elm);
    /// emplace element at back (backSpare must be > 0!)
    template<class... ARGS> void emplaceBack(ARGS&&... args);
    /// pop back element
    TYPE popBack();

    /// push element at front (frontSpare must be > 0!)
    void pushFront(const TYPE& elm);
    /// move element at front (frontSpare must be > 0!)
    void pushFront(TYPE&& elm);
    /// emplace element at front (frontSpare must be > 0!)
    template<class... ARGS> void emplaceFront(ARGS&&... args);
    /// pop front element
    TYPE popFront();
    
    /// prepare insertion by making room and returning pointer to insert to
    TYPE* prepareInsert(int32 index, bool& outSlotConstructed);
    /// copy-insert element at position, keep other elements in their previous order
    void insert(int32 index, const TYPE& elm);
    /// move-insert element at position, keep other elements in their previous order
    void insert(int32 index, TYPE&& elm);

    /// erase element at index, keeps array ordering
    void erase(int32 index);
    /// erase element at index, swap in element from back or front, messes up ordering
    void eraseSwap(int32 index);
    /// erase element at index, always swap-in element from the back
    void eraseSwapBack(int32 index);
    /// erase element at index, always swap-in element from the front
    void eraseSwapFront(int32 index);
    
    /// move elements towards front for insertion, return pointer to free insertion slot
    TYPE* moveInsertFront(int32 index);
    /// move elements towards end for insertion, return pointer to free insertion slot
    TYPE* moveInsertBack(int32 index);
    /// erase element by moving elements from front
    void moveEraseFront(int32 index);
    /// erase element by moving elements from back
    void moveEraseBack(int32 index);
    
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
    copyConstruct(rhs.elmStart, this->elmStart, rhs.size());
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
elementBuffer<TYPE>::~elementBuffer() {
    this->destroy();
}
    
//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::operator=(const elementBuffer<TYPE>& rhs) {
    if (&rhs != this) {
        this->destroy();
        const int32 newSize = rhs.size();
        if (newSize > 0)
        {
            this->alloc(rhs.size(), 0);
            copyConstruct(rhs.elmStart, this->elmStart, rhs.size());
            this->elmEnd = this->elmStart + rhs.size();
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::operator=(elementBuffer<TYPE>&& rhs) {
    if (&rhs != this) {
        this->bufStart = rhs.bufStart;
        this->bufEnd   = rhs.bufEnd;
        this->elmStart = rhs.elmStart;
        this->elmEnd   = rhs.elmEnd;
        rhs.bufStart = 0;
        rhs.bufEnd   = 0;
        rhs.elmStart = 0;
        rhs.elmEnd   = 0;
    }
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
    o_assert(newCapacity > 0);
    if (this->capacity() == newCapacity) {
        return;
    }
    const int32 curSize = this->size();
    o_assert((newFrontSpare + curSize) <= newCapacity);

    // allocate new buffer
    const int32 newBufSize = newCapacity * sizeof(TYPE);
    TYPE* newBuffer = (TYPE*) Memory::Alloc(newBufSize);
    TYPE* newElmStart = newBuffer + newFrontSpare;
    
    // need to move any elements?
    if (curSize > 0) {
        // move-construct elements over to new buffer
        TYPE* src = this->elmStart;
        TYPE* dst = newElmStart;
        for (int i = 0; i < curSize; i++) {
            new(dst++) TYPE(std::move(*src));
            // must still call destructor on move-source
            src++->~TYPE();
        }
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
    if (0 != this->elmStart) {
        for (TYPE* ptr = this->elmStart; ptr < this->elmEnd; ptr++) {
            ptr->~TYPE();
        }
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
elementBuffer<TYPE>::copyConstruct(const TYPE* from, TYPE* to, int32 num) {
    o_assert(!overlaps(from, to, num));
    for (int i = 0; i < num; i++) {
        new(to++) TYPE(*from++);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::copyAssign(const TYPE* from, TYPE* to, int32 num) {
    o_assert(!overlaps(from, to, num));
    for (int i = 0; i < num; i++) {
        *to++ = *from++;
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushBack(const TYPE& elm) {
    // NOTE: this will fail if there is no spare space at the back,
    // use insert(size(), elm) which will move towards front if possible
    o_assert((nullptr != this->elmEnd) && (this->elmEnd < this->bufEnd));
    new(this->elmEnd++) TYPE(elm);
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushBack(TYPE&& elm) {
    // NOTE: this will fail if there is no spare space at the back,
    // use insert(size(), elm) which will move towards front if possible
    o_assert((nullptr != this->elmEnd) && (this->elmEnd < this->bufEnd));
    new(this->elmEnd++) TYPE(std::move(elm));
}

//------------------------------------------------------------------------------
template<class TYPE> template<class... ARGS> void
elementBuffer<TYPE>::emplaceBack(ARGS&&... args) {
    // NOTE: this will fail if there is no spare space at the back,
    // use insert(size(), elm) which will move towards front if possible
    o_assert((nullptr != this->elmEnd) && (this->elmEnd < this->bufEnd));
    new(this->elmEnd++) TYPE(std::forward<ARGS>(args)...);
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushFront(const TYPE& elm) {
    // NOTE: this will fail if there is no spare space at the front,
    // use insert(0, elm) which will move towards back if possible
    o_assert((nullptr != this->elmStart) && (this->elmStart > this->bufStart));
    new(--this->elmStart) TYPE(elm);
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushFront(TYPE&& elm) {
    // NOTE: this will fail if there is no spare space at the front,
    // use insert(0, elm) which will move towards back if possible
    o_assert((nullptr != this->elmStart) && (this->elmStart > this->bufStart));
    new(--this->elmStart) TYPE(std::move(elm));
}

//------------------------------------------------------------------------------
template<class TYPE> template<class... ARGS> void
elementBuffer<TYPE>::emplaceFront(ARGS&&... args) {
    // NOTE: this will fail if there is no spare space at the front,
    // use insert(0, elm) which will move towards back if possible
    o_assert((nullptr != this->elmStart) && (this->elmStart > this->bufStart));
    new(--this->elmStart) TYPE(std::forward<ARGS>(args)...);
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
elementBuffer<TYPE>::moveInsertFront(int32 index) {
    // free a slot for insertion by moving the elements
    // at and before it towards the front
    // the freed slot will NOT be deconstructed!
    o_assert(this->elmStart > this->bufStart);
    o_assert((index >= 0) && (index <= this->size()));
    
    new(this->elmStart - 1) TYPE(std::move(*this->elmStart));
    for (TYPE* ptr = this->elmStart; ptr < this->elmStart + index - 1; ptr++) {
        *ptr = std::move(*(ptr + 1));
    }
    this->elmStart--;
    return this->elmStart + index;
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
elementBuffer<TYPE>::moveInsertBack(int32 index) {
    // free a slot for insertion by moving the elements
    // after it towards the back
    // the freed slot will NOT be deconstructed!
    o_assert(this->elmEnd < (this->bufEnd - 1));
    o_assert((index >= 0) && (index < this->size()));
    
    new(this->elmEnd) TYPE(std::move(*(this->elmEnd-1)));
    for (TYPE* ptr = this->elmEnd - 1; ptr > (this->elmStart+index); ptr--) {
        *ptr = std::move(*(ptr - 1));
    }
    this->elmEnd++;
    return this->elmStart + index;
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::moveEraseFront(int32 index) {
    // erase a slot by moving elements from the front
    o_assert((index >= 0) && (index < this->size()));
    for (TYPE* ptr = this->elmStart + index; ptr > this->elmStart; ptr--) {
        *ptr = std::move(*(ptr - 1));
    }
    // must deconstruct the previous front element
    this->elmStart->~TYPE();
    this->elmStart++;
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::moveEraseBack(int32 index) {
    // erase a slot by moving elements from the back
    o_assert((index >= 0) && (index < this->size()));
    for (TYPE* ptr = this->elmStart + index; ptr < (this->elmEnd - 1); ptr++) {
        *ptr = std::move(*(ptr + 1));
    }
    // must deconstruct the previous back element
    this->elmEnd--;
    this->elmEnd->~TYPE();
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
elementBuffer<TYPE>::prepareInsert(int32 index, bool& outSlotConstructed) {

    // this method will return a pointer to an empty, destructed slot!

    outSlotConstructed = true;
    const int32 size = this->size();
    if (index == size) {
        // special case insert at end of array
        if (this->elmEnd < this->bufEnd) {
            // still room to spare at end
            outSlotConstructed = false;
            return this->elmEnd++;
        }
        else if (this->elmStart > this->bufStart) {
            // make room by moving towards front (this should always be faster then reallocating)
            return this->moveInsertFront(index);
        }
        // can't happen
    }
    else if (0 == index) {
        // special case insert at front
        if (this->elmStart > this->bufStart) {
            // still room at front, don't need to move
            outSlotConstructed = false;
            return --this->elmStart;
        }
        else if (this->elmEnd < this->bufEnd) {
            // no room at front, must make room by moving towards end
            return this->moveInsertBack(index);
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
                return this->moveInsertFront(index);
            }
            else {
                // must move towards end
                o_assert(this->elmEnd < this->bufEnd);
                return this->moveInsertBack(index);
            }
        }
        else {
            // prefer move toward end
            if (this->elmEnd < this->bufEnd) {
                // move towards back
                return this->moveInsertBack(index);
            }
            else {
                // must move towards front
                o_assert(this->elmStart > this->bufStart);
                return this->moveInsertFront(index);
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
    bool slotConstructed = true;
    TYPE* ptr = this->prepareInsert(index, slotConstructed);
    if (slotConstructed) {
        *ptr = elm;
    }
    else {
        new(ptr) TYPE(elm);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::insert(int32 index, TYPE&& elm) {
    bool slotConstructed = true;
    TYPE* ptr = this->prepareInsert(index, slotConstructed);
    if (slotConstructed) {
        *ptr = elm;
    }
    else {
        new(ptr) TYPE(std::move(elm));
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::erase(int32 index) {
    const int32 size = this->size();
    o_assert((index >= 0) && (index < size) && (0 != this->elmStart));
    
    if (0 == index) {
        // special case: first element
        this->elmStart->~TYPE();
        this->elmStart++;
    }
    else if (index == (size - 1)) {
        // special case: last element
        this->elmEnd--;
        this->elmEnd->~TYPE();
    }
    else {
        // move the smaller amount of elements to fill the gap
        if (index < (size>>1)) {
            // move front elements
            this->moveEraseFront(index);
        }
        else {
            // move back elements
            this->moveEraseBack(index);
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::eraseSwap(int32 index) {
    const int32 size = this->size();
    o_assert((index >= 0) && (index < size) && (0 != this->elmStart));
    
    if (0 == index) {
        // special case: first element
        this->elmStart->~TYPE();
        this->elmStart++;
    }
    else if (index == (size - 1)) {
        // special case: last element
        this->elmEnd--;
        this->elmEnd->~TYPE();
    }
    else {
        // either swap in the first or last element (keep frontSpare and backSpare balanced)
        TYPE* ptr = this->elmStart + index;
        if (this->frontSpare() > this->backSpare()) {
            // swap-in element from back
            *ptr = std::move(*--this->elmEnd);
            this->elmEnd->~TYPE();
        }
        else {
            // swap-in element from front
            *ptr = std::move(*this->elmStart);
            this->elmStart->~TYPE();
            this->elmStart++;
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::eraseSwapBack(int32 index) {
    const int32 size = this->size();
    o_assert((index >= 0) && (index < size) && (0 != this->elmStart));
    if (index == (size - 1)) {
        // special case: last element
        this->elmEnd--;
        this->elmEnd->~TYPE();
    }
    else {
        // swap-in element from back
        TYPE* ptr = this->elmStart + index;
        *ptr = std::move(*--this->elmEnd);
        this->elmEnd->~TYPE();
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::eraseSwapFront(int32 index) {
    const int32 size = this->size();
    o_assert((index >= 0) && (index < size) && (0 != this->elmStart));
    
    if (0 == index) {
        // special case: first element
        this->elmStart->~TYPE();
        this->elmStart++;
    }
    else {
        // swap-in element from front
        TYPE* ptr = this->elmStart + index;
        *ptr = std::move(*this->elmStart);
        this->elmStart->~TYPE();
        this->elmStart++;
    }
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE
elementBuffer<TYPE>::popBack() {
    o_assert(this->elmEnd > this->elmStart);
    TYPE val(std::move(*--this->elmEnd));
    this->elmEnd->~TYPE();
    return val;
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE
elementBuffer<TYPE>::popFront() {
    o_assert(this->elmStart < this->elmEnd);
    TYPE val(std::move(*this->elmStart));
    this->elmStart->~TYPE();
    this->elmStart++;
    return val;
}



} // namespace Core
} // namespace Oryol
//------------------------------------------------------------------------------
