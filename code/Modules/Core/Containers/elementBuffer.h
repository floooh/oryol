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
    int frontSpare() const;
    /// get number of free slots at back
    int backSpare() const;
    /// get overall spare (frontSpare + backSpare)
    int spare() const;
    /// get number of valid elements
    int size() const;
    /// get overall capacity
    int capacity() const;
    
    /// access element by index
    TYPE& operator[](int index);
    /// read-access element by index
    const TYPE& operator[](int index) const;
    /// get front element (r/w)
    TYPE& front();
    /// get front element (r/o)
    const TYPE& front() const;
    /// get back element (r/w)
    TYPE& back();
    /// get back element (r/o)
    const TYPE& back() const;
    
    /// allocate, grow or shrink the elementBuffer
    void alloc(int capacity, int frontSpare);
    /// destroy all
    void destroy();
    /// destroy element at pointer
    static void destroyElement(TYPE* ptr);
    /// clear content
    void clear();
    
    /// test if to pointer is within [from, from+num]
    static bool overlaps(const TYPE* from, const TYPE* to, int num);
    /// copy-construct element range
    static void copyConstruct(const TYPE* from, TYPE* to, int num);
    /// copy-assign element range UNTESTED
    static void copyAssign(const TYPE* from, TYPE* to, int num);
    
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
    TYPE* prepareInsert(int index, bool& outSlotConstructed);
    /// copy-insert element at position, keep other elements in their previous order
    void insert(int index, const TYPE& elm);
    /// move-insert element at position, keep other elements in their previous order
    void insert(int index, TYPE&& elm);

    /// erase element at index, keeps array ordering
    void erase(int index);
    /// erase element at index, swap in element from back or front, messes up ordering
    void eraseSwap(int index);
    /// erase element at index, always swap-in element from the back
    void eraseSwapBack(int index);
    /// erase element at index, always swap-in element from the front
    void eraseSwapFront(int index);
    /// erase a range of elements
    void eraseRange(int index, int num);
    
    /// move elements towards front for insertion, return pointer to free insertion slot
    TYPE* moveInsertFront(int index);
    /// move elements towards end for insertion, return pointer to free insertion slot
    TYPE* moveInsertBack(int index);
    /// erase element by moving elements from front
    void moveEraseFront(int index);
    /// erase element by moving elements from back
    void moveEraseBack(int index);

    /// C++ begin
    TYPE* _begin();
    /// C++ begin
    const TYPE* _begin() const;
    /// C++ end
    TYPE* _end();
    /// C++ end
    const TYPE* _end() const;

    TYPE* buf;          // buffer start;
    int cap;            // buffer capacity (num elements)
    int start;          // index of first valid element in buffer
    int end;            // index of one-past-last valid element in buffer
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
        o_assert_range_dbg(rhs.start, rhs.cap);
        o_assert_dbg((rhs.start + rhs.size()) <= rhs.cap);
        o_assert_range_dbg(this->start, this->cap);
        o_assert_dbg((this->start + this->size()) <= this->cap);
        copyConstruct(&rhs.buf[rhs.start], &this->buf[this->start], rhs.size());
        this->end = this->start + rhs.size();
        o_assert_dbg(rhs.end <= rhs.cap);
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
    // reset rhs to default-constructed state
    rhs.buf = nullptr;
    rhs.cap = 0;
    rhs.start = 0;
    rhs.end = 0;
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
        const int newSize = rhs.size();
        if (newSize > 0)
        {
            this->alloc(rhs.size(), 0);
            o_assert_range_dbg(rhs.start, rhs.cap);
            o_assert_dbg((rhs.start + rhs.size()) <= rhs.cap);
            o_assert_range_dbg(this->start, this->cap);
            o_assert_dbg((this->start + this->size()) <= this->cap);
            copyConstruct(&rhs.buf[rhs.start], &this->buf[this->start], rhs.size());
            this->end = this->start + rhs.size();
            o_assert_dbg(rhs.end <= rhs.cap);
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
template<class TYPE> int
elementBuffer<TYPE>::frontSpare() const {
    return this->start;
}

//------------------------------------------------------------------------------
template<class TYPE> int
elementBuffer<TYPE>::backSpare() const {
    return this->cap - this->end;
}

//------------------------------------------------------------------------------
template<class TYPE> int
elementBuffer<TYPE>::spare() const {
    return this->cap - this->size();
}
    
//------------------------------------------------------------------------------
template<class TYPE> int
elementBuffer<TYPE>::size() const {
    return this->end-this->start;
}

//------------------------------------------------------------------------------
template<class TYPE> int
elementBuffer<TYPE>::capacity() const {
    return this->cap;
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
elementBuffer<TYPE>::operator[](int index) {
    o_assert_dbg((index >= 0) && (index < this->size()));
    o_assert_dbg(this->buf);
    o_assert_range_dbg(this->start+index, this->cap);
    return this->buf[this->start + index];
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
elementBuffer<TYPE>::operator[](int index) const {
    o_assert_dbg((index >= 0) && (index < this->size()));
    o_assert_dbg(this->buf);
    o_assert_range_dbg(this->start+index, this->cap);
    return this->buf[this->start + index];
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
elementBuffer<TYPE>::front() {
    o_assert((this->start != this->end) && this->buf);
    o_assert_range_dbg(this->start, this->cap);
    return this->buf[this->start];
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
elementBuffer<TYPE>::front() const {
    o_assert((this->start != this->end) && this->buf);
    o_assert_range_dbg(this->start, this->cap);
    return this->buf[this->start];
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
elementBuffer<TYPE>::back() {
    o_assert((this->start != this->end) && this->buf);
    o_assert_range_dbg(this->end-1, this->cap);
    return this->buf[this->end - 1];
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
elementBuffer<TYPE>::back() const {
    o_assert((this->start != this->end) && this->buf);
    o_assert_range_dbg(this->end-1, this->cap);
    return this->buf[this->end - 1];
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::alloc(int newCapacity, int newStart) {
    o_assert_dbg(newCapacity > 0);
    if (this->cap == newCapacity) {
        return;
    }
    const int curSize = this->size();
    o_assert_dbg((newStart + curSize) <= newCapacity);

    // allocate new buffer
    const int newBufSize = newCapacity * sizeof(TYPE);
    TYPE* newBuffer = (TYPE*) Memory::Alloc(newBufSize);
    TYPE* newElmStart = newBuffer + newStart;
    
    // need to move any elements?
    if (curSize > 0) {
        // move-construct elements over to new buffer
        o_assert_dbg(this->buf);
        o_assert_range_dbg(this->start, this->cap);
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
        for (int i = this->start; i < this->end; i++) {
            o_assert_range_dbg(i, this->cap);
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
        for (int i = this->start; i < this->end; i++) {
            o_assert_range_dbg(i, this->cap);
            this->buf[i].~TYPE();
        }
    }
    this->end = this->start;
}

//------------------------------------------------------------------------------
template<class TYPE> bool
elementBuffer<TYPE>::overlaps(const TYPE* from, const TYPE* to, int num) {
    return (to >= from) && (to < (from + num));
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::copyConstruct(const TYPE* from, TYPE* to, int num) {
    o_assert_dbg(!overlaps(from, to, num));
    for (int i = 0; i < num; i++) {
        new(to++) TYPE(*from++);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::copyAssign(const TYPE* from, TYPE* to, int num) {
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
    o_assert_dbg(this->buf);
    o_assert_range_dbg(this->end, this->cap);
    new(&this->buf[this->end++]) TYPE(elm);
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushBack(TYPE&& elm) {
    // NOTE: this will fail if there is no spare space at the back,
    // use insert(size(), elm) which will move towards front if possible
    o_assert_dbg(this->buf);
    o_assert_range_dbg(this->end, this->cap);
    new(&this->buf[this->end++]) TYPE(std::move(elm));
}

//------------------------------------------------------------------------------
template<class TYPE> template<class... ARGS> void
elementBuffer<TYPE>::emplaceBack(ARGS&&... args) {
    // NOTE: this will fail if there is no spare space at the back,
    // use insert(size(), elm) which will move towards front if possible
    o_assert_dbg(this->buf);
    o_assert_range_dbg(this->end, this->cap);
    new(&this->buf[this->end++]) TYPE(std::forward<ARGS>(args)...);
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushFront(const TYPE& elm) {
    // NOTE: this will fail if there is no spare space at the front,
    // use insert(0, elm) which will move towards back if possible
    o_assert_dbg(this->buf && (this->start > 0) && (this->start <= this->cap));
    new(&this->buf[--this->start]) TYPE(elm);
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::pushFront(TYPE&& elm) {
    // NOTE: this will fail if there is no spare space at the front,
    // use insert(0, elm) which will move towards back if possible
    o_assert_dbg(this->buf && (this->start > 0) && (this->start <= this->cap));
    new(&this->buf[--this->start]) TYPE(std::move(elm));
}

//------------------------------------------------------------------------------
template<class TYPE> template<class... ARGS> void
elementBuffer<TYPE>::emplaceFront(ARGS&&... args) {
    // NOTE: this will fail if there is no spare space at the front,
    // use insert(0, elm) which will move towards back if possible
    o_assert_dbg(this->buf && (this->start > 0) && (this->start <= this->cap));
    new(&this->buf[--this->start]) TYPE(std::forward<ARGS>(args)...);
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
elementBuffer<TYPE>::moveInsertFront(int index) {
    // free a slot for insertion by moving the elements
    // at and before it towards the front
    // the freed slot will NOT be deconstructed!
    o_assert_dbg(this->buf);
    o_assert_dbg((index >= 0) && (index <= this->size()));
    
    o_assert_dbg((this->start > 0) && (this->start < this->cap));
    new(&this->buf[this->start-1]) TYPE(std::move(this->buf[start]));
    for (int i = this->start; i < (this->start + index - 1); i++) {
        o_assert_dbg((i >= 0) && ((i+1) < this->cap));
        this->buf[i] = std::move(this->buf[i+1]);
    }
    this->start--;
    o_assert_range_dbg(this->start+index, this->cap);
    return &this->buf[this->start + index];
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
elementBuffer<TYPE>::moveInsertBack(int index) {
    // free a slot for insertion by moving the elements
    // after it towards the back
    // the freed slot will NOT be deconstructed!
    o_assert_dbg(this->buf);
    o_assert_dbg((index >= 0) && (index < this->size()));

    o_assert_dbg((this->end > 0) && (this->end < this->cap));
    new(&this->buf[this->end]) TYPE(std::move(this->buf[this->end-1]));
    for (int i = this->end - 1; i > (this->start + index); i--) {
        o_assert_dbg(((i-1) >= 0) && (i < this->cap));
        this->buf[i] = std::move(this->buf[i-1]);
    }
    this->end++;
    o_assert_range_dbg(this->start+index, this->cap);
    return &this->buf[this->start + index];
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::moveEraseFront(int index) {
    // erase a slot by moving elements from the front
    o_assert_dbg(this->buf && (index >= 0) && (index < this->size()));
    for (int i = this->start + index; i > this->start; i--) {
        o_assert_dbg(((i-1) >= 0) && (i < this->cap));
        this->buf[i] = std::move(this->buf[i - 1]);
    }
    // must deconstruct the previous front element
    o_assert_range_dbg(this->start, this->cap);
    this->buf[this->start++].~TYPE();
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::moveEraseBack(int index) {
    // erase a slot by moving elements from the back
    o_assert_dbg(this->buf && (index >= 0) && (index < this->size()));
    for (int i = this->start + index; i < (this->end - 1); i++) {
        o_assert_dbg((i >= 0) && ((i+1) < this->cap));
        this->buf[i] = std::move(this->buf[i + 1]);
    }
    // must deconstruct the previous back element
    o_assert_dbg((this->end > 0) && (this->end <= this->cap));
    this->buf[--this->end].~TYPE();
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
elementBuffer<TYPE>::prepareInsert(int index, bool& outSlotConstructed) {

    // this method will return a pointer to an empty, destructed slot!

    outSlotConstructed = true;
    const int size = this->size();
    if (index == size) {
        // special case insert at end of array
        if (this->end < this->cap) {
            // still room to spare at end
            outSlotConstructed = false;
            o_assert_range_dbg(this->end, this->cap);
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
            o_assert_dbg((this->start > 0) && (this->start <= this->cap));
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
        // a "normal" insert, decide whether to move forward or backward,
        // try to keep the front- and back-spare size balanced
        o_assert_range_dbg(this->start+index, this->cap);
        TYPE* ptr = &this->buf[this->start + index];
        if (this->frontSpare() > this->backSpare()) {
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
elementBuffer<TYPE>::insert(int index, const TYPE& elm) {
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
elementBuffer<TYPE>::insert(int index, TYPE&& elm) {
    bool slotConstructed = true;
    TYPE* ptr = this->prepareInsert(index, slotConstructed);
    if (slotConstructed) {
        *ptr = std::move(elm);
    }
    else {
        new(ptr) TYPE(std::move(elm));
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::erase(int index) {
    const int size = this->size();
    o_assert_dbg(this->buf && (index >= 0) && (index < size));
    
    if (index == (size - 1)) {
        // special case: last element
        o_assert_dbg((this->end > 0) && (this->end <= this->cap));
        this->buf[--this->end].~TYPE();
    }
    else if (0 == index) {
        // special case: first element
        o_assert_range_dbg(this->start, this->cap);
        this->buf[this->start++].~TYPE();
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
elementBuffer<TYPE>::eraseSwap(int index) {
    const int size = this->size();
    o_assert_dbg(this->buf && (index >= 0) && (index < size));
    
    if (0 == index) {
        // special case: first element
        o_assert_range_dbg(this->start, this->cap);
        this->buf[this->start++].~TYPE();
    }
    else if (index == (size - 1)) {
        // special case: last element
        o_assert_dbg((this->end > 0) && (this->end <= this->cap));
        this->buf[--this->end].~TYPE();
    }
    else {
        // either swap in the first or last element (keep frontSpare and backSpare balanced)
        if (this->frontSpare() > this->backSpare()) {
            // swap-in element from back
            o_assert_range_dbg(this->start+index, this->cap);
            o_assert_dbg((this->end > 0) && (this->end <= this->cap));
            this->buf[this->start + index] = std::move(this->buf[--this->end]);
            this->buf[this->end].~TYPE();
        }
        else {
            // swap-in element from front
            o_assert_range_dbg(this->start+index, this->cap);
            o_assert_range_dbg(this->start, this->cap);
            this->buf[this->start + index] = std::move(this->buf[this->start]);
            this->buf[this->start++].~TYPE();
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::eraseSwapBack(int index) {
    const int size = this->size();
    o_assert_dbg(this->buf && (index >= 0) && (index < size));
    if (index == (size - 1)) {
        // special case: last element
        o_assert_dbg((this->end > 0) && (this->end <= this->cap));
        this->buf[--this->end].~TYPE();
    }
    else {
        // swap-in element from back
        o_assert_range_dbg(this->start+index, this->cap);
        o_assert_dbg((this->end > 0) && (this->end <= this->cap));
        this->buf[this->start + index] = std::move(this->buf[--this->end]);
        this->buf[this->end].~TYPE();
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::eraseSwapFront(int index) {
    o_assert_dbg(this->buf && (index >= 0) && (index < this->size()));
    if (0 == index) {
        // special case: first element
        o_assert_range_dbg(this->start, this->cap);
        this->buf[this->start++].~TYPE();
    }
    else {
        // swap-in element from front
        o_assert_range_dbg(this->start+index, this->cap);
        o_assert_range_dbg(this->start, this->cap);
        this->buf[this->start + index] = std::move(this->buf[this->start]);
        this->buf[this->start++].~TYPE();
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
elementBuffer<TYPE>::eraseRange(int index, int num) {
    o_assert_dbg(this->buf && (index>=0) && ((index+num) <= this->size()) && (num >= 0));
    if (0 == num) {
        return;
    }
    int i = this->start + index;
    for (; i < (this->end - num); i++) {
        o_assert_range_dbg(i, this->cap);
        o_assert_range_dbg(i+num, this->cap);
        this->buf[i] = std::move(this->buf[i + num]);
    }
    for (; i < this->end; i++) {
        o_assert_range_dbg(i, this->cap);
        this->buf[i].~TYPE();
    }
    this->end -= num;
    o_assert_dbg(this->end >= this->start);
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE
elementBuffer<TYPE>::popBack() {
    o_assert_dbg(this->buf && (this->end > this->start));
    o_assert_dbg((this->end > 0) && (this->end <= this->cap));
    TYPE val(std::move(this->buf[--this->end]));
    this->buf[this->end].~TYPE();
    return val;
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE
elementBuffer<TYPE>::popFront() {
    o_assert_dbg(this->buf && (this->start < this->end));
    o_assert_range_dbg(this->start, this->cap);
    TYPE val(std::move(this->buf[this->start]));
    this->buf[this->start++].~TYPE();
    return val;
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE*
elementBuffer<TYPE>::_begin() {
    if (this->buf) {
        // NOTE: the returned pointer may point to invalid memory!
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
        // NOTE: the returned pointer may point to invalid memory!
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
        // NOTE: the returned pointer may point to invalid memory!
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
        // NOTE: the returned pointer may point to invalid memory!
        return &this->buf[this->end];
    }
    else {
        return nullptr;
    }
}

} // namespace _priv
} // namespace Oryol

