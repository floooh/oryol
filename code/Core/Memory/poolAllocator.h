#pragma once
//------------------------------------------------------------------------------
/*
    private class, don't use!
 
    Thread-safe pool allocator with placement-new/delete. Uses 32-bit
    tags with a unique-count masked-in for its forward-linked list 
    instead of pointers because of the ABA problem (which I was actually 
    running into with many threads and high object reuse).
*/
#include "Core/Types.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace Core {
    
template<class TYPE, int POOL_SIZE> class poolAllocator {
public:
    /// constructor
    poolAllocator();
    /// destructor
    ~poolAllocator();
    
    /// allocate and construct an object of type T
    template<typename... ARGS> TYPE* Create(ARGS&&... args);
    /// delete and free an object
    void Destroy(TYPE* obj);
    
    /// get pool size
    int32 GetPoolSize() const;
    
private:
    enum class nodeState : uint8 {
        init, free, used,
    };
    
    typedef uint32 nodeTag;    // [16bit counter] | [16bit elm_index])
    static const uint32 invalidTag = 0xFFFFFFFF;

    struct node {
        nodeTag next;          // tag of next node
        nodeTag myTag;        // my own tag
        nodeState state;       // current state
        uint8 padding[16 - (2*sizeof(nodeTag) + sizeof(state))];      // pad to 16 bytes
    };

    /// pop a new node from the free-list, return 0 if empty
    node* pop();
    /// push a node onto the free-list
    void push(node*);
    /// allocate pool and populate free-list
    void allocPool();
    /// get node address from a tag
    node* addressFromTag(nodeTag tag) const;
    /// get tag from a node address
    nodeTag tagFromAddress(node* n) const;
    /// test if a pointer is owned by this allocator (SLOW)
    bool isOwned(TYPE* obj) const;
    
    int32 elmSize;                     // offset to next element in bytes
    std::atomic<uint32> uniqueCount;
    std::atomic<nodeTag> head;         // free-list head
    uint8* pool;
};

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
poolAllocator<TYPE,POOL_SIZE>::poolAllocator()
{
    static_assert(sizeof(node) == 16, "pool_allocator::node should be 16 bytes!");
    static_assert(POOL_SIZE <= ((1<<16) - 1), "POOL_SIZE must be < (1<<16)-1!");

    this->pool = 0;
    this->elmSize = Memory::RoundUp(sizeof(node) + sizeof(TYPE), sizeof(node));
    o_assert((this->elmSize & (sizeof(node) - 1)) == 0);
    o_assert(this->elmSize >= (int32)(2*sizeof(node)));
    this->uniqueCount = 0;
    this->head = invalidTag;
    this->allocPool();
    o_assert(0 != pool);
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
poolAllocator<TYPE,POOL_SIZE>::~poolAllocator() {
    Memory::Free(this->pool);
    this->pool = 0;
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
typename poolAllocator<TYPE,POOL_SIZE>::node*
poolAllocator<TYPE,POOL_SIZE>::addressFromTag(nodeTag tag) const {
    uint32 elmIndex = tag & 0xFFFF;
    o_assert(elmIndex < POOL_SIZE);
    uint8* ptr = this->pool + elmIndex * elmSize;
    return (node*) ptr;
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
typename poolAllocator<TYPE,POOL_SIZE>::nodeTag
poolAllocator<TYPE,POOL_SIZE>::tagFromAddress(node* n) const {
    o_assert(nullptr != n);
    nodeTag tag = n->myTag;
    #if ORYOL_ALLOCATOR_DEBUG
    o_assert(n == addressFromTag(tag));
    #endif
    return tag;
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE> void
poolAllocator<TYPE,POOL_SIZE>::allocPool() {
    
    o_assert(0 == this->pool);
    
    // allocate new pool
    const size_t poolByteSize = POOL_SIZE * this->elmSize;
    this->pool = (uint8*) Memory::Alloc(poolByteSize);
    Memory::Clear(this->pool, poolByteSize);
    
    // populate the free stack
    for (int32 elmIndex = (POOL_SIZE - 1); elmIndex >= 0; elmIndex--) {
        uint8* ptr = this->pool + elmIndex * this->elmSize;
        node* nodePtr = (node*) ptr;
        nodePtr->next  = invalidTag;
        nodePtr->myTag = elmIndex;
        nodePtr->state = nodeState::init;
        this->push(nodePtr);
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE> void
poolAllocator<TYPE,POOL_SIZE>::push(node* newHead) {
    
    // see http://www.boost.org/doc/libs/1_53_0/boost/lockfree/stack.hpp
    o_assert((nodeState::init == newHead->state) || (nodeState::used == newHead->state));
    
    o_assert(invalidTag == newHead->next);
    #if ORYOL_ALLOCATOR_DEBUG
    Memory::Fill((void*) (newHead + 1), sizeof(TYPE), 0xAA);
    #endif
    
    newHead->state = nodeState::free;
    newHead->myTag = (newHead->myTag & 0x0000FFFF) | (++this->uniqueCount & 0xFFFF) << 16;
    nodeTag oldHeadTag = this->head.load(std::memory_order_relaxed);
    for (;;) {
        newHead->next = oldHeadTag;
        if (this->head.compare_exchange_weak(oldHeadTag, newHead->myTag)) {
            break;
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
typename poolAllocator<TYPE,POOL_SIZE>::node*
poolAllocator<TYPE,POOL_SIZE>::pop()
{
    // see http://www.boost.org/doc/libs/1_53_0/boost/lockfree/stack.hpp
    for (;;) {
        nodeTag oldHeadTag = this->head.load(std::memory_order_consume);
        if (invalidTag == oldHeadTag) {
            return nullptr;
        }
        nodeTag newHeadTag = this->addressFromTag(oldHeadTag)->next;
        if (this->head.compare_exchange_weak(oldHeadTag, newHeadTag)) {
            o_assert(invalidTag != oldHeadTag);
            node* nodePtr = this->addressFromTag(oldHeadTag);
            o_assert(nodeState::free == nodePtr->state);
            #if ORYOL_ALLOCATOR_DEBUG
            Memory::Fill((void*) (nodePtr+ 1), sizeof(TYPE), 0xBB);
            #endif
            nodePtr->next = invalidTag;
            nodePtr->state = nodeState::used;
            return nodePtr;
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
template<typename... ARGS> TYPE*
poolAllocator<TYPE, POOL_SIZE>::Create(ARGS&&... args) {
    
    // pop a new node from the free-stack
    node* n = this->pop();
    o_assert(nullptr != n);
    
    // construct with placement new
    void* objPtr = (void*) (n + 1);
    TYPE* obj = new(objPtr) TYPE(std::forward<ARGS>(args)...);
    o_assert(obj == objPtr);
    return obj;
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE> bool
poolAllocator<TYPE, POOL_SIZE>::isOwned(TYPE* obj) const {
    const uint8* start = this->pool;
    const uint8* end = this->pool + POOL_SIZE * this->elm_size;
    const uint8* ptr = (uint8*) obj;
    return ((ptr >= start) && (ptr < end));
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE> void
poolAllocator<TYPE, POOL_SIZE>::Destroy(TYPE* obj) {
    
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

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE> int32
poolAllocator<TYPE, POOL_SIZE>::GetPoolSize() const {
    return POOL_SIZE;
}

} // namespace Core
} // namespace Oryol
