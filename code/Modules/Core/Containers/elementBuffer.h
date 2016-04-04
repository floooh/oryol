#pragma once
//------------------------------------------------------------------------------
/*
    @class Oryol::_priv::elementBuffer
    @ingroup _priv
 
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
#include <utility>
#include "Core/Types.h"
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"

//------------------------------------------------------------------------------
namespace Oryol {
namespace _priv {

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
    /// get overall spare (frontSpare + backSpare)
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

    /// C++ begin
    TYPE* _begin();
    /// C++ begin
    const TYPE* _begin() const;
    /// C++ end
    TYPE* _end();
    /// C++ end
    const TYPE* _end() const;

    TYPE* buf;          // buffer start;
    int32 cap;          // buffer capacity (num elements)
    int32 start;        // index of first valid element in buffer
    int32 end;          // index of one-past-last valid element in buffer
};

//------------------------------------------------------------------------------
template<class TYPE>
elementBuffer<TYPE>::elementBuffer() :
buf(nullptr),
cap(0),
start(0),
end(0)
{
    // empty
}

//------------------------------------------------------------------------------
template<class TYPE>
elementBuffer<TYPE>::elementBuffer(const elementBuffer& rhs) :
buf(nullptr),
cap(0),
start(0),
end(0)
{
    if (rhs.buf) {
        this->alloc(rhs.size(), 0);
        copyConstruct(&rhs.buf[rhs.start], &this->buf[this->start], rhs.size());
        this->end = this->start + rhs.size();
    }
}

//------------------------------------------------------------------------------
template<class TYPE>
elementBuffer<TYPE>::elementBuffer(elementBuffer&& rhs) :
buf(rhs.buf),
cap(rhs.cap),
start(rhs.start),
end(rhs.end)
{
    rhs.buf = nullptr;
    rhs.cap = 0;
    rhs.start = InvalidIndex;
    rhs.end = InvalidIndex;
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
            copyConstruct(&rhs.buf[rhs.start], &this->buf[this->start], rhs.size());
            this->end = this->start + rhs.size();
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::operator=(elementBuffer<TYPE>&& rhs) {
    if (&rhs != this) {
        this->destroy();
        this->buf   = rhs.buf;
        this->cap   = rhs.cap;
        this->start = rhs.start;
        this->end   = rhs.end;
        rhs.buf   = nullptr;
        rhs.cap   = 0;
        rhs.start = 0;
        rhs.end   = 0;
    }
}

//------------------------------------------------------------------------------
template<class TYPE> int32
elementBuffer<TYPE>::frontSpare() const {
    return this->start;
}

//------------------------------------------------------------------------------
template<class TYPE> int32
elementBuffer<TYPE>::backSpare() const {
    return this->cap - this->end;
}

//------------------------------------------------------------------------------
template<class TYPE> int32
elementBuffer<TYPE>::spare() const {
    return this->cap - this->size();
}
    
//------------------------------------------------------------------------------
template<class TYPE> int32
elementBuffer<TYPE>::size() const {
    return this->end-this->start;
}

//------------------------------------------------------------------------------
template<class TYPE> int32
elementBuffer<TYPE>::capacity() const {
    return this->cap;
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
elementBuffer<TYPE>::operator[](int32 index) {
    o_assert_dbg((index >= 0) && (index < this->size()));
    o_assert_dbg(this->buf);
    return this->buf[this->start + index];
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
elementBuffer<TYPE>::operator[](int32 index) const {
    o_assert_dbg((index >= 0) && (index < this->size()));
    o_assert_dbg(this->buf);
    return this->buf[this->start + index];
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
elementBuffer<TYPE>::front() {
    o_assert((this->start != this->end) && this->buf);
    return this->buf[this->start];
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
elementBuffer<TYPE>::front() const {
    o_assert((this->start != this->end) && this->buf);
    return this->buf[this->start];
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
elementBuffer<TYPE>::back() {
    o_assert((this->start != this->end) && this->buf);
    return this->buf[this->end - 1];
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
elementBuffer<TYPE>::back() const {
    o_assert((this->start != this->end) && this->buf);
    return this->buf[this->end - 1];
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::alloc(int32 newCapacity, int32 newStart) {
    o_assert_dbg(newCapacity > 0);
    if (this->cap == newCapacity) {
        return;
    }
    const int32 curSize = this->size();
    o_assert_dbg((newStart + curSize) <= newCapacity);

    // allocate new buffer
    const int32 newBufSize = newCapacity * sizeof(TYPE);
    TYPE* newBuffer = (TYPE*) Memory::Alloc(newBufSize);
    TYPE* newElmStart = newBuffer + newStart;
    
    // need to move any elements?
    if (curSize > 0) {
        // move-construct elements over to new buffer
        o_assert_dbg(this->buf);
        TYPE* src = &this->buf[this->start];
        TYPE* dst = newElmStart;
        for (int i = 0; i < curSize; i++) {
            new(dst++) TYPE(std::move(*src));
            // must still call destructor on move-source
            src++->~TYPE();
        }
    }
    
    // need to free old buffer?
    if (nullptr != this->buf) {
        Memory::Free(this->buf);
    }
    
    // replace pointers
    this->buf   = newBuffer;
    this->cap   = newCapacity;
    this->start = newStart;
    this->end   = newStart + curSize;
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::destroy() {
    // destroy elements and free buffer
    if (this->buf) {
        for (int32 i = this->start; i < this->end; i++) {
            this->buf[i].~TYPE();
        }
        Memory::Free(this->buf);
    }
    this->buf = nullptr;
    this->cap = 0;
    this->start = 0;
    this->end = 0;
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::destroyElement(TYPE* elm) {
    elm->~TYPE();
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::clear() {
    if (this->buf) {
        for (int32 i = this->start; i < this->end; i++) {
            this->buf[i].~TYPE();
        }
    }
    this->end = this->start;
}

//------------------------------------------------------------------------------
template<class TYPE> bool
elementBuffer<TYPE>::overlaps(const TYPE* from, const TYPE* to, int32 num) {
    return (to >= from) && (to < (from + num));
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::copyConstruct(const TYPE* from, TYPE* to, int32 num) {
    o_assert_dbg(!overlaps(from, to, num));
    for (int i = 0; i < num; i++) {
        new(to++) TYPE(*from++);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::copyAssign(const TYPE* from, TYPE* to, int32 num) {
    o_assert_dbg(!overlaps(from, to, num));
    for (int i = 0; i < num; i++) {
        *to++ = *from++;
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushBack(const TYPE& elm) {
    // NOTE: this will fail if there is no spare space at the back,
    // use insert(size(), elm) which will move towards front if possible
    o_assert_dbg(this->buf && (this->end < this->cap));
    new(&this->buf[this->end++]) TYPE(elm);
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushBack(TYPE&& elm) {
    // NOTE: this will fail if there is no spare space at the back,
    // use insert(size(), elm) which will move towards front if possible
    o_assert_dbg(this->buf && (this->end < this->cap));
    new(&this->buf[this->end++]) TYPE(std::move(elm));
}

//------------------------------------------------------------------------------
template<class TYPE> template<class... ARGS> void
elementBuffer<TYPE>::emplaceBack(ARGS&&... args) {
    // NOTE: this will fail if there is no spare space at the back,
    // use insert(size(), elm) which will move towards front if possible
    o_assert_dbg(this->buf && (this->end < this->cap));
    new(&this->buf[this->end++]) TYPE(std::forward<ARGS>(args)...);
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushFront(const TYPE& elm) {
    // NOTE: this will fail if there is no spare space at the front,
    // use insert(0, elm) which will move towards back if possible
    o_assert_dbg(this->buf && (this->start > 0));
    new(&this->buf[--this->start]) TYPE(elm);
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushFront(TYPE&& elm) {
    // NOTE: this will fail if there is no spare space at the front,
    // use insert(0, elm) which will move towards back if possible
    o_assert_dbg(this->buf && (this->start > 0));
    new(&this->buf[--this->start]) TYPE(std::move(elm));
}

//------------------------------------------------------------------------------
template<class TYPE> template<class... ARGS> void
elementBuffer<TYPE>::emplaceFront(ARGS&&... args) {
    // NOTE: this will fail if there is no spare space at the front,
    // use insert(0, elm) which will move towards back if possible
    o_assert_dbg(this->buf && (this->start > 0));
    new(&this->buf[--this->start]) TYPE(std::forward<ARGS>(args)...);
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
elementBuffer<TYPE>::moveInsertFront(int32 index) {
    // free a slot for insertion by moving the elements
    // at and before it towards the front
    // the freed slot will NOT be deconstructed!
    o_assert_dbg(this->buf && (this->start > 0));
    o_assert_dbg((index >= 0) && (index <= this->size()));
    
    new(&this->buf[this->start-1]) TYPE(std::move(this->buf[start]));
    for (int32 i = this->start; i < (this->start + index - 1); i++) {
        this->buf[i] = std::move(this->buf[i+1]);
    }
    this->start--;
    return &this->buf[this->start + index];
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
elementBuffer<TYPE>::moveInsertBack(int32 index) {
    // free a slot for insertion by moving the elements
    // after it towards the back
    // the freed slot will NOT be deconstructed!
    o_assert_dbg(this->buf && (this->end > 0) && (this->end < this->cap));
    o_assert_dbg((index >= 0) && (index < this->size()));
    
    new(&this->buf[this->end]) TYPE(std::move(this->buf[this->end-1]));
    for (int32 i = this->end - 1; i > (this->start + index); i--) {
        this->buf[i] = std::move(this->buf[i-1]);
    }
    this->end++;
    return &this->buf[this->start + index];
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::moveEraseFront(int32 index) {
    // erase a slot by moving elements from the front
    o_assert_dbg(this->buf && (index >= 0) && (index < this->size()));
    for (int32 i = this->start + index; i > this->start; i--) {
        this->buf[i] = std::move(this->buf[i - 1]);
    }
    // must deconstruct the previous front element
    this->buf[this->start++].~TYPE();
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::moveEraseBack(int32 index) {
    // erase a slot by moving elements from the back
    o_assert_dbg(this->buf && (index >= 0) && (index < this->size()));
    for (int32 i = this->start + index; i < (this->end - 1); i++) {
        this->buf[i] = std::move(this->buf[i + 1]);
    }
    // must deconstruct the previous back element
    this->buf[--this->end].~TYPE();
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
elementBuffer<TYPE>::prepareInsert(int32 index, bool& outSlotConstructed) {

    // this method will return a pointer to an empty, destructed slot!

    outSlotConstructed = true;
    const int32 size = this->size();
    if (index == size) {
        // special case insert at end of array
        if (this->end < this->cap) {
            // still room to spare at end
            outSlotConstructed = false;
            return &this->buf[this->end++];
        }
        else if (this->start > 0) {
            // make room by moving towards front (this should always be faster then reallocating)
            return this->moveInsertFront(index);
        }
        else {
            // can't happen
            o_assert_dbg(false);
        }
    }
    else if (0 == index) {
        // special case insert at front
        if (this->start > 0) {
            // still room at front, don't need to move
            outSlotConstructed = false;
            return &this->buf[--this->start];
        }
        else if (this->end < this->cap) {
            // no room at front, must make room by moving towards end
            return this->moveInsertBack(index);
        }
        else {
            // can't happen
            o_assert_dbg(false);
        }
    }
    else {
        // a "normal" insert, decide whether to move forward or backward
        TYPE* ptr = &this->buf[this->start + index];
        if (index < (size>>1)) {
            // prefer move toward front
            if (this->start > 0) {
                // move towards front
                return this->moveInsertFront(index);
            }
            else {
                // must move towards end
                o_assert_dbg(this->end < this->cap);
                return this->moveInsertBack(index);
            }
        }
        else {
            // prefer move toward end
            if (this->end < this->cap) {
                // move towards back
                return this->moveInsertBack(index);
            }
            else {
                // must move towards front
                o_assert_dbg(this->start > 0);
                return this->moveInsertFront(index);
            }
        }
        return ptr;
    }
    // can't happen
    o_assert(false);
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
    o_assert_dbg(this->buf && (index >= 0) && (index < size));
    
    if (0 == index) {
        // special case: first element
        this->buf[this->start++].~TYPE();
    }
    else if (index == (size - 1)) {
        // special case: last element
        this->buf[--this->end].~TYPE();
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
    o_assert_dbg(this->buf && (index >= 0) && (index < size));
    
    if (0 == index) {
        // special case: first element
        this->buf[this->start++].~TYPE();
    }
    else if (index == (size - 1)) {
        // special case: last element
        this->buf[--this->end].~TYPE();
    }
    else {
        // either swap in the first or last element (keep frontSpare and backSpare balanced)
        if (this->frontSpare() > this->backSpare()) {
            // swap-in element from back
            this->buf[this->start + index] = std::move(this->buf[--this->end]);
            this->buf[this->end].~TYPE();
        }
        else {
            // swap-in element from front
            this->buf[this->start + index] = std::move(this->buf[this->start]);
            this->buf[this->start++].~TYPE();
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::eraseSwapBack(int32 index) {
    const int32 size = this->size();
    o_assert_dbg(this->buf && (index >= 0) && (index < size));
    if (index == (size - 1)) {
        // special case: last element
        this->buf[--this->end].~TYPE();
    }
    else {
        // swap-in element from back
        this->buf[this->start + index] = std::move(this->buf[--this->end]);
        this->buf[this->end].~TYPE();
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::eraseSwapFront(int32 index) {
    o_assert_dbg(this->buf && (index >= 0) && (index < this->size()));
    if (0 == index) {
        // special case: first element
        this->buf[this->start++].~TYPE();
    }
    else {
        // swap-in element from front
        this->buf[this->start + index] = std::move(this->buf[this->start]);
        this->buf[this->start++].~TYPE();
    }
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE
elementBuffer<TYPE>::popBack() {
    o_assert_dbg(this->buf && (this->end > this->start));
    TYPE val(std::move(this->buf[--this->end]));
    this->buf[this->end].~TYPE();
    return val;
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE
elementBuffer<TYPE>::popFront() {
    o_assert_dbg(this->buf && (this->start < this->end));
    TYPE val(std::move(this->buf[this->start]));
    this->buf[this->start++].~TYPE();
    return val;
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
elementBuffer<TYPE>::_begin() {
    if (this->buf) {
        return &this->buf[this->start];
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE*
elementBuffer<TYPE>::_begin() const {
    if (this->buf) {
        return &this->buf[this->start];
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
elementBuffer<TYPE>::_end() {
    if (this->buf) {
        return &this->buf[this->end];
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE*
elementBuffer<TYPE>::_end() const {
    if (this->buf) {
        return &this->buf[this->end];
    }
    else {
        return nullptr;
    }
}

} // namespace _priv
} // namespace Oryol
//------------------------------------------------------------------------------
