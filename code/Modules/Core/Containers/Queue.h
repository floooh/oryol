#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Queue
    @ingroup Core
    @brief a FIFO queue
*/
#include "Core/Config.h"
#include "Core/Containers/elementBuffer.h"

namespace Oryol {

template<class TYPE> class Queue {
public:
    /// default constructor
    Queue();
    /// copy constructor
    Queue(const Queue& rhs);
    /// move constructor
    Queue(Queue&& rhs);
    /// destructor
    ~Queue();
    
    /// copy-assignment
    void operator=(const Queue& rhs);
    /// move-assignment
    void operator=(Queue&& rhs);
    
    /// set allocation strategy
    void SetAllocStrategy(int minGrow, int maxGrow);
    /// initialize to a fixed capacity (guarantees that no re-allocs happen)
    void SetFixedCapacity(int fixedCapacity);
    /// get min-grow value
    int GetMinGrow() const;
    /// get max-grow value
    int GetMaxGrow() const;
    /// get number of elements in array
    int Size() const;
    /// return true if empty
    bool Empty() const;
    /// get capacity of queue
    int Capacity() const;
    /// get number of free slots at dequeue-side
    int SpareDequeue() const;
    /// get number of free slots at enqueue-side
    int SpareEnqueue() const;
    
    /// increase capacity to hold at least numElements more elements
    void Reserve(int numElements);
    /// clear the queue
    void Clear();
    
    /// read/write access to first element
    TYPE& Front();
    /// read-only access to first element
    const TYPE& Front() const;
    /// read/write access to last element
    TYPE& Back();
    /// read-only access to first element
    const TYPE& Back() const;
    
    /// copy-enqueue an element
    void Enqueue(const TYPE& elm);
    /// move-enqueue an element
    void Enqueue(TYPE&& elm);
    /// construct-enqueue an element
    template<class... ARGS> void Enqueue(ARGS&&... args);

    /// dequeue an element
    TYPE Dequeue();
    /// dequeue into existing element
    void Dequeue(TYPE& outElm);
    
private:
    /// destroy contained resource
    void destroy();
    /// copy from other queue
    void copy(const Queue& rhs);
    /// move from other queue
    void move(Queue&& rhs);
    /// reallocate with new capacity
    void adjustCapacity(int newCapacity);
    /// grow to make room
    void grow();
    /// move elements to the front
    void moveToFront();
    /// common checks before enqueuing
    void checkEnqueue();
    
    _priv::elementBuffer<TYPE> buffer;
    int minGrow;
    int maxGrow;
};

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::destroy() {
    this->minGrow = 0;
    this->maxGrow = 0;
    this->buffer.destroy();
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::copy(const Queue<TYPE>& rhs) {
    this->minGrow = rhs.minGrow;
    this->maxGrow = rhs.maxGrow;
    this->buffer = rhs.buffer;
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::move(Queue&& rhs) {
    this->minGrow = rhs.minGrow;
    this->maxGrow = rhs.maxGrow;
    this->buffer  = std::move(rhs.buffer);
    // NOTE: don't reset minGrow/maxGrow, rhs is empty, but still a valid object!
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::adjustCapacity(int newCapacity) {
    this->buffer.alloc(newCapacity, 0);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::grow() {
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

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::moveToFront() {
    const int num = this->buffer.size();
    if (num > 0) {
        o_assert_dbg(this->buffer.buf);
        const TYPE* from = this->buffer._begin();
        TYPE* to = this->buffer.buf;
        for (int i = 0; i < num; i++) {
            new(to) TYPE(std::move(*from));
            from->~TYPE();
            to++;
            from++;
        }
    }
    this->buffer.start = 0;
    this->buffer.end = this->buffer.start + num;
}

//------------------------------------------------------------------------------
template<class TYPE>
Queue<TYPE>::Queue() :
minGrow(ORYOL_CONTAINER_DEFAULT_MIN_GROW),
maxGrow(ORYOL_CONTAINER_DEFAULT_MAX_GROW) {
    // empty
}

//------------------------------------------------------------------------------
template<class TYPE>
Queue<TYPE>::Queue(const Queue& rhs) {
    this->copy(rhs);
}

//------------------------------------------------------------------------------
template<class TYPE>
Queue<TYPE>::Queue(Queue&& rhs) {
    this->move(std::move(rhs));
}

//------------------------------------------------------------------------------
template<class TYPE>
Queue<TYPE>::~Queue() {
    this->destroy();
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::operator=(const Queue& rhs) {
    /// @todo: this should be optimized when rhs.size() < this->capacity()!
    if (&rhs != this) {
        this->destroy();
        this->copy(rhs);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::operator=(Queue&& rhs) {
    /// @todo: this should be optimized when rhs.size() < this->capacity()!
    if (&rhs != this) {
        this->destroy();
        this->move(std::move(rhs));
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::SetAllocStrategy(int minGrow_, int maxGrow_) {
    this->minGrow = minGrow_;
    this->maxGrow = maxGrow_;
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::SetFixedCapacity(int fixedCapacity) {
    this->minGrow = 0;
    this->maxGrow = 0;
    if (fixedCapacity > this->buffer.capacity()) {
        this->adjustCapacity(fixedCapacity);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> int
Queue<TYPE>::GetMinGrow() const {
    return this->minGrow;
}

//------------------------------------------------------------------------------
template<class TYPE> int
Queue<TYPE>::GetMaxGrow() const {
    return this->maxGrow;
}

//------------------------------------------------------------------------------
template<class TYPE> int
Queue<TYPE>::Size() const {
    return this->buffer.size();
}

//------------------------------------------------------------------------------
template<class TYPE> int
Queue<TYPE>::Capacity() const {
    return this->buffer.capacity();
}

//------------------------------------------------------------------------------
template<class TYPE> bool
Queue<TYPE>::Empty() const {
    return this->buffer.size() == 0;
}

//------------------------------------------------------------------------------
template<class TYPE> int
Queue<TYPE>::SpareDequeue() const {
    return this->buffer.frontSpare();
}

//------------------------------------------------------------------------------
template<class TYPE> int
Queue<TYPE>::SpareEnqueue() const {
    return this->buffer.backSpare();
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::Reserve(int numElements) {
    int newCapacity = this->buffer.size() + numElements;
    if (newCapacity > this->buffer.capacity()) {
        this->adjustCapacity(newCapacity);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::Clear() {
    this->buffer.clear();
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::checkEnqueue() {
    // if there are currently no elements in the queue,
    // then this is a good time to move the start
    // to the front
    if (0 == this->buffer.size()) {
        this->moveToFront();
    }
    if (this->buffer.backSpare() == 0) {
        // no space at back, if there's front-space, move elements to front
        if (this->buffer.frontSpare() > 0) {
            this->moveToFront();
        }
        else {
            // no space at front either, need to grow
            this->grow();
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::Enqueue(const TYPE& elm) {
    this->checkEnqueue();
    this->buffer.pushBack(elm);
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::Enqueue(TYPE&& elm) {
    this->checkEnqueue();
    this->buffer.pushBack(std::move(elm));
}

//------------------------------------------------------------------------------
template<class TYPE> template<class... ARGS> void
Queue<TYPE>::Enqueue(ARGS&&... args) {
    this->checkEnqueue();
    this->buffer.emplaceBack(std::forward<ARGS>(args)...);
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE
Queue<TYPE>::Dequeue() {
    o_assert_dbg(this->buffer.size() > 0);
    return std::move(this->buffer.popFront());
}

//------------------------------------------------------------------------------
template<class TYPE> void
Queue<TYPE>::Dequeue(TYPE& outElm) {
    o_assert_dbg(this->buffer.size() > 0);
    outElm = std::move(this->buffer.popFront());
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
Queue<TYPE>::Front() {
    return this->buffer.front();
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
Queue<TYPE>::Front() const {
    return this->buffer.front();
}

//------------------------------------------------------------------------------
template<class TYPE> TYPE&
Queue<TYPE>::Back() {
    return this->buffer.back();
}

//------------------------------------------------------------------------------
template<class TYPE> const TYPE&
Queue<TYPE>::Back() const {
    return this->buffer.back();
}

} // namespace Oryol
