#pragma once
//------------------------------------------------------------------------------
/**
    @class oryol::memory::pool_allocator
    
    Thread-safe pool allocator with placement-new/delete. Uses 32-bit
    tags with a unique-count masked-in for its forward-linked list 
    instead of pointers because of the ABA problem (which I was actually 
    running into with many threads and high object reuse).
*/
#include "core/types.h"
#include "core/memory/memory.h"

namespace oryol {
namespace memory {
    
template<class TYPE, int POOL_SIZE> class pool_allocator {
public:
    /// constructor
    pool_allocator();
    /// destructor
    ~pool_allocator();
    
    /// allocate and construct an object of type T
    template<typename... ARGS> TYPE* create(ARGS&&... args);
    /// delete and free an object
    void destroy(TYPE* obj);
    
    /// get pool size
    int32 get_pool_size() const;
    /// test if a pointer is owned by this allocator (SLOW)
    bool is_owned(TYPE* obj) const;
    
private:
    enum class node_state : uint8 {
        init, free, used,
    };
    
    typedef uint32 node_tag;    // [16bit counter] | [16bit elm_index])
    static const uint32 invalid_tag = 0xFFFFFFFF;

    struct node {
        node_tag next;          // tag of next node
        node_tag my_tag;        // my own tag
        node_state state;       // current state
        uint8 padding[16 - (2*sizeof(node_tag) + sizeof(state))];      // pad to 16 bytes
    };

    /// pop a new node from the free-list, return 0 if empty
    node* pop();
    /// push a node onto the free-list
    void push(node*);
    /// allocate pool and populate free-list
    void alloc_pool();
    /// get node address from a tag
    node* address_from_tag(node_tag tag) const;
    /// get tag from a node address
    node_tag tag_from_address(node* n) const;
    
    int32 elm_size;                     // offset to next element in bytes
    std::atomic<uint32> unique_count;
    std::atomic<node_tag> head;         // free-list head
    uint8* pool;
};

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
pool_allocator<TYPE,POOL_SIZE>::pool_allocator()
{
    static_assert(sizeof(node) == 16, "pool_allocator::node should be 16 bytes!");
    static_assert(POOL_SIZE <= ((1<<16) - 1), "POOL_SIZE must be < (1<<16)-1!");

    pool = 0;
    elm_size = memory::roundup(sizeof(node) + sizeof(TYPE), sizeof(node));
    o_assert((elm_size & (sizeof(node) - 1)) == 0);
    #if ORYOL_ALLOCATOR_DEBUG
    o_assert(elm_size >= 2*sizeof(node));
    #endif
    unique_count = 0;
    head = invalid_tag;
    this->alloc_pool();
    o_assert(0 != pool);
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
pool_allocator<TYPE,POOL_SIZE>::~pool_allocator() {
    memory::free(pool);
    pool = 0;
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
typename pool_allocator<TYPE,POOL_SIZE>::node* pool_allocator<TYPE,POOL_SIZE>::address_from_tag(node_tag tag) const {
    uint32 elm_index = tag & 0xFFFF;
    o_assert(elm_index < POOL_SIZE);
    uint8* ptr = pool + elm_index * elm_size;
    return (node*) ptr;
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
typename pool_allocator<TYPE,POOL_SIZE>::node_tag pool_allocator<TYPE,POOL_SIZE>::tag_from_address(node* n) const {
    o_assert(nullptr != n);
    node_tag tag = n->my_tag;
    o_assert(n == address_from_tag(tag));
    return tag;
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
void pool_allocator<TYPE,POOL_SIZE>::alloc_pool() {
    
    o_assert(0 == pool);
    
    // allocate new pool
    const size_t pool_byte_size = POOL_SIZE * elm_size;
    pool = (uint8*) memory::alloc(pool_byte_size);
    memory::clear(pool, pool_byte_size);
    
    // populate the free stack
    for (int32 elm_index = (POOL_SIZE - 1); elm_index >= 0; elm_index--) {
        uint8* ptr = pool + elm_index * elm_size;
        node* node_ptr = (node*) ptr;
        node_ptr->next   = invalid_tag;
        node_ptr->my_tag = elm_index;
        node_ptr->state  = node_state::init;
        push(node_ptr);
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
void pool_allocator<TYPE,POOL_SIZE>::push(node* new_head) {
    
    // see http://www.boost.org/doc/libs/1_53_0/boost/lockfree/stack.hpp
    o_assert((node_state::init == new_head->state) || (node_state::used == new_head->state));
    
    o_assert(invalid_tag == new_head->next);
    #if ORYOL_ALLOCATOR_DEBUG
    memory::fill((void*) (new_head + 1), sizeof(TYPE), 0xAA);
    #endif
    
    new_head->state  = node_state::free;
    new_head->my_tag = (new_head->my_tag & 0x0000FFFF) | (++unique_count & 0xFFFF) << 16;
    node_tag old_head_tag = head.load(std::memory_order_relaxed);
    for (;;) {
        new_head->next = old_head_tag;
        if (head.compare_exchange_weak(old_head_tag, new_head->my_tag)) {
            break;
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
typename pool_allocator<TYPE,POOL_SIZE>::node* pool_allocator<TYPE,POOL_SIZE>::pop()
{
    // see http://www.boost.org/doc/libs/1_53_0/boost/lockfree/stack.hpp
    for (;;) {
        node_tag old_head_tag = head.load(std::memory_order_consume);
        if (invalid_tag == old_head_tag) {
            return nullptr;
        }
        node_tag new_head_tag = address_from_tag(old_head_tag)->next;
        if (head.compare_exchange_weak(old_head_tag, new_head_tag)) {
            o_assert(invalid_tag != old_head_tag);
            node* node_ptr = address_from_tag(old_head_tag);
            o_assert(node_state::free == node_ptr->state);
            #if ORYOL_ALLOCATOR_DEBUG
            memory::fill((void*) (node_ptr+ 1), sizeof(TYPE), 0xBB);
            #endif
            node_ptr->next = invalid_tag;
            node_ptr->state = node_state::used;
            return node_ptr;
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
template<typename... ARGS>
TYPE* pool_allocator<TYPE, POOL_SIZE>::create(ARGS&&... args) {
    
    // pop a new node from the free-stack
    node* n = pop();
    o_assert(nullptr != n);
    
    // construct with placement new
    void* obj_ptr = (void*) (n + 1);
    TYPE* obj = new(obj_ptr) TYPE(std::forward<ARGS>(args)...);
    o_assert(obj == obj_ptr);
    return obj;
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
bool pool_allocator<TYPE, POOL_SIZE>::is_owned(TYPE* obj) const {
    const uint8* start = pool;
    const uint8* end = pool + POOL_SIZE * elm_size;
    const uint8* ptr = (uint8*) ptr;
    return ((ptr >= start) && (ptr < end));
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
void pool_allocator<TYPE, POOL_SIZE>::destroy(TYPE* obj) {
    
    #if ORYOL_ALLOCATOR_DEBUG
    // make sure this object has been allocated by us
    o_assert(is_owned(obj));
    #endif
    
    // call destructor on obj
    obj->~TYPE();
    
    // push the pool element back on the free-stack
    node* n = ((node*)obj) - 1;
    push(n);
}

//------------------------------------------------------------------------------
template<class TYPE, int POOL_SIZE>
int pool_allocator<TYPE, POOL_SIZE>::get_pool_size() const {
    return POOL_SIZE;
}

} // namespace memory
} // namespace oryol