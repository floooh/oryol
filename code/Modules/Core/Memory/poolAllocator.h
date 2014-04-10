#pragma once
//------------------------------------------------------------------------------
/*
    private class, don't use!
 
    Thread-safe pool allocator with placement-new/delete. Uses 32-bit
    tags with a unique-count masked-in for its forward-linked list 
    instead of pointers because of the ABA problem (which I was actually 
    running into with many threads and high object reuse). The pool
    is split into up to 256 "puddles", where each puddle can hold
    up to 256 elements. When no elements are in the free list, 
    a new puddle is allocated. Thus one pool can hold up to
    65536 elements.
*/
#include <atomic>
#include <utility>
#include "Core/Types.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace Core {
    
template<class TYPE> class poolAllocator {
public:
    /// constructor
    poolAllocator();
    /// destructor
    ~poolAllocator();
    
    /// allocate and construct an object of type T
    template<typename... ARGS> TYPE* Create(ARGS&&... args);
    /// delete and free an object
    void Destroy(TYPE* obj);
    
private:
    enum class nodeState : uint8 {
        init, free, used,
    };
    
    typedef uint32 nodeTag;    // [16bit counter] | [8bit puddle index ] | [8bit elm_index])
    static const uint32 invalidTag = 0xFFFFFFFF;

    struct node {
        nodeTag next;          // tag of next node
        nodeTag myTag;        // my own tag
        nodeState state;       // current state
        uint8 padding[16 - (2*sizeof(nodeTag) + sizeof(nodeState))];      // pad to 16 bytes
    };

    /// pop a new node from the free-list, return 0 if empty
    node* pop();
    /// push a node onto the free-list
    void push(node*);
    /// allocate a new puddle and add entries to free-list
    void allocPuddle();
    /// get node address from a tag
    node* addressFromTag(nodeTag tag) const;
    /// get tag from a node address
    nodeTag tagFromAddress(node* n) const;
    /// test if a pointer is owned by this allocator (SLOW)
    bool isOwned(TYPE* obj) const;
    
    static const uint32 MaxNumPuddles = 256;
    static const uint32 NumPuddleElements = 256;
    
    int32 elmSize;                      // offset to next element in bytes

    #if ORYOL_HAS_ATOMIC
        std::atomic<uint32> uniqueCount;
        std::atomic<nodeTag> head;          // free-list head
        std::atomic<uint32> numPuddles;     // current number of puddles
    #else
        uint32 uniqueCount;
        nodeTag head;
        uint32 numPuddles;
    #endif
    uint8* puddles[MaxNumPuddles];
};

//------------------------------------------------------------------------------
template<class TYPE>
poolAllocator<TYPE>::poolAllocator()
{
    static_assert(sizeof(node) == 16, "pool_allocator::node should be 16 bytes!");

    Memory::Clear(this->puddles, sizeof(this->puddles));
    this->numPuddles = 0;
    this->elmSize = Memory::RoundUp(sizeof(node) + sizeof(TYPE), sizeof(node));
    o_assert((this->elmSize & (sizeof(node) - 1)) == 0);
    o_assert(this->elmSize >= (int32)(2*sizeof(node)));
    this->uniqueCount = 0;
    this->head = invalidTag;
}

//------------------------------------------------------------------------------
template<class TYPE>
poolAllocator<TYPE>::~poolAllocator() {

    const uint32 num = this->numPuddles;
    for (uint32 i = 0; i < num; i++) {
        Memory::Free(this->puddles[i]);
        this->puddles[i] = 0;
    }
}

//------------------------------------------------------------------------------
template<class TYPE>
typename poolAllocator<TYPE>::node*
poolAllocator<TYPE>::addressFromTag(nodeTag tag) const {
    uint32 elmIndex = tag & 0xFF;
    uint32 puddleIndex = (tag & 0xFF00) >> 8;
    uint8* ptr = this->puddles[puddleIndex] + elmIndex * elmSize;
    return (node*) ptr;
}

//------------------------------------------------------------------------------
template<class TYPE>
typename poolAllocator<TYPE>::nodeTag
poolAllocator<TYPE>::tagFromAddress(node* n) const {
    o_assert(nullptr != n);
    nodeTag tag = n->myTag;
    #if ORYOL_ALLOCATOR_DEBUG
    o_assert(n == addressFromTag(tag));
    #endif
    return tag;
}

//------------------------------------------------------------------------------
template<class TYPE> void
poolAllocator<TYPE>::allocPuddle() {

    // increment the puddle-counter (this must happen first because the
    // method can be called from different threads
    #if ORYOL_HAS_ATOMIC
        uint32 newPuddleIndex = this->numPuddles.fetch_add(1, std::memory_order_relaxed);
    #else 
        uint32 newPuddleIndex = this->numPuddles++;
    #endif
    o_assert(newPuddleIndex < MaxNumPuddles);
    
    // allocate new puddle
    const uint32 puddleByteSize = NumPuddleElements * this->elmSize;
    this->puddles[newPuddleIndex] = (uint8*) Memory::Alloc(puddleByteSize);
    Memory::Clear(this->puddles[newPuddleIndex], puddleByteSize);
    
    // populate the free stack
    for (int32 elmIndex = (NumPuddleElements - 1); elmIndex >= 0; elmIndex--) {
        uint8* ptr = this->puddles[newPuddleIndex] + elmIndex * this->elmSize;
        node* nodePtr = (node*) ptr;
        nodePtr->next  = invalidTag;
        nodePtr->myTag = (newPuddleIndex << 8) | elmIndex;
        nodePtr->state = nodeState::init;
        this->push(nodePtr);
    }
}

//------------------------------------------------------------------------------
template<class TYPE> void
poolAllocator<TYPE>::push(node* newHead) {
    
    // see http://www.boost.org/doc/libs/1_53_0/boost/lockfree/stack.hpp
    o_assert((nodeState::init == newHead->state) || (nodeState::used == newHead->state));
    
    o_assert(invalidTag == newHead->next);
    #if ORYOL_ALLOCATOR_DEBUG
    Memory::Fill((void*) (newHead + 1), sizeof(TYPE), 0xAA);
    #endif
    
    newHead->state = nodeState::free;
    newHead->myTag = (newHead->myTag & 0x0000FFFF) | (++this->uniqueCount & 0xFFFF) << 16;
    #if ORYOL_HAS_ATOMIC
        nodeTag oldHeadTag = this->head.load(std::memory_order_relaxed);
        for (;;) {
            newHead->next = oldHeadTag;
            if (this->head.compare_exchange_weak(oldHeadTag, newHead->myTag)) {
                break;
            }
        }
    #else
        nodeTag oldHeadTag = this->head;
        newHead->next = oldHeadTag;
        this->head = newHead->myTag;
    #endif
}

//------------------------------------------------------------------------------
template<class TYPE>
typename poolAllocator<TYPE>::node*
poolAllocator<TYPE>::pop()
{
    // see http://www.boost.org/doc/libs/1_53_0/boost/lockfree/stack.hpp
    for (;;) {
        #if ORYOL_HAS_ATOMIC
            nodeTag oldHeadTag = this->head.load(std::memory_order_consume);
        #else 
            nodeTag oldHeadTag = this->head;
        #endif
        if (invalidTag == oldHeadTag) {
            return nullptr;
        }
        nodeTag newHeadTag = this->addressFromTag(oldHeadTag)->next;
        #if ORYOL_HAS_ATOMIC
        if (this->head.compare_exchange_weak(oldHeadTag, newHeadTag)) {
        #else
        this->head = newHeadTag;
        #endif
            o_assert(invalidTag != oldHeadTag);
            node* nodePtr = this->addressFromTag(oldHeadTag);
            o_assert(nodeState::free == nodePtr->state);
            #if ORYOL_ALLOCATOR_DEBUG
            Memory::Fill((void*) (nodePtr+ 1), sizeof(TYPE), 0xBB);
            #endif
            nodePtr->next = invalidTag;
            nodePtr->state = nodeState::used;
            return nodePtr;
        #if ORYOL_HAS_ATOMIC
        }
        #endif
    }
}

//------------------------------------------------------------------------------
template<class TYPE>
template<typename... ARGS> TYPE*
poolAllocator<TYPE>::Create(ARGS&&... args) {
    
    // pop a new node from the free-stack
    node* n = this->pop();
    if (nullptr == n) {
        // need to allocate a new puddle
        this->allocPuddle();
        n = this->pop();
    }
    o_assert(nullptr != n);
    
    // construct with placement new
    void* objPtr = (void*) (n + 1);
    TYPE* obj = new(objPtr) TYPE(std::forward<ARGS>(args)...);
    o_assert(obj == objPtr);
    return obj;
}

//------------------------------------------------------------------------------
template<class TYPE> bool
poolAllocator<TYPE>::isOwned(TYPE* obj) const {
    const uint32 num = this->numPuddles;
    for (uint32 i = 0; i < num; i++) {
        const uint8* start = this->puddles[i];
        const uint8* end = this->puddles[i] + NumPuddleElements * this->elmSize;
        const uint8* ptr = (uint8*) obj;
        if ((ptr >= start) && (ptr < end)) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
template<class TYPE> void
poolAllocator<TYPE>::Destroy(TYPE* obj) {
    
    #if ORYOL_ALLOCATOR_DEBUG
    // make sure this object has been allocated by us
    o_assert(this->isOwned(obj));
    #endif
    
    // call destructor on obj
    obj->~TYPE();
    
    // push the pool element back on the free-stack
    node* n = ((node*)obj) - 1;
    push(n);
}

} // namespace Core
} // namespace Oryol
