#pragma once
//------------------------------------------------------------------------------
/**
    @class oryol::memory::pool_allocator
    
    Thread-safe pool allocator with placement-new/delete.
    
    FIXME: growable not yet implemented (currently always growable)
*/
#include "core/types.h"
#include "core/memory/memory.h"

namespace oryol {
namespace memory {
    
template<class TYPE, int CHUNK_SIZE> class pool_allocator {
public:
    /// constructor
    pool_allocator(bool growable);
    /// destructor
    ~pool_allocator();
    
    /// allocate and construct an object of type T
    template<typename... ARGS> TYPE* create(ARGS&&... args);
    /// delete and free an object
    void destroy(TYPE* obj);
    
    /// test if a pointer is owned by this allocator (SLOW)
    bool is_owned(TYPE* obj) const;
    
private:
    enum class node_state : uint8 {
        init, free, used,
    };

    struct node {
        node* next;                             // pointer to next node
        node_state state;                       // current state (for debugging)
        uint8 padding[16 - (sizeof(node*) + sizeof(node_state))];      // pad to 16 bytes
    };

    /// pop a new node from the free-list, return 0 if empty
    node* pop();
    /// push a node onto the free-list
    void push(node*);
    /// allocate a new chunk and populate free-list
    void alloc_chunk();

    bool growable;
    int32 elm_size;             // offset to next element in sizeof(node)
    std::atomic<node*> head;    // free-list head
    std::vector<void*> chunks;  // allocated chunks new chunk is allocated when free list is empty)
    mutable std::mutex chunk_mutex;     // must be taken when a new chunk is allocated
};

//------------------------------------------------------------------------------
template<class TYPE, int CHUNK_SIZE>
pool_allocator<TYPE,CHUNK_SIZE>::pool_allocator(bool g) :
    growable(g)
{
    static_assert(sizeof(node) == 16, "pool_allocator::node should be 16 bytes!");

    elm_size = memory::roundup(sizeof(node) + sizeof(TYPE), sizeof(node));
    o_assert((elm_size & (sizeof(node) - 1)) == 0);
    elm_size /= sizeof(node);
    #if ORYOL_ALLOCATOR_DEBUG
    o_assert(elm_size > 0);
    #endif
    head = 0;
    chunks.reserve(128);
}

//------------------------------------------------------------------------------
template<class TYPE, int CHUNK_SIZE>
pool_allocator<TYPE,CHUNK_SIZE>::~pool_allocator() {
    // free memory chunks
    for (void* p : chunks) {
        memory::free(p);
    }
    chunks.clear();
}

//------------------------------------------------------------------------------
template<class TYPE, int CHUNK_SIZE>
void pool_allocator<TYPE,CHUNK_SIZE>::alloc_chunk() {
    
    // allocate new chunk
    const size_t chunk_byte_size = CHUNK_SIZE * elm_size * sizeof(node);
    node* new_chunk = (node*) memory::alloc(chunk_byte_size);
    memory::clear(new_chunk, chunk_byte_size);
    
    // this requires some locking
    {
        std::lock_guard<std::mutex> lock(chunk_mutex);
        chunks.push_back(new_chunk);
    }
    
    // push new nodes into free list
    for (int32 i = (CHUNK_SIZE-1) * elm_size; i >= 0; i -= elm_size) {
        new_chunk[i].state = node_state::init;
        push(&(new_chunk[i]));
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int CHUNK_SIZE>
void pool_allocator<TYPE,CHUNK_SIZE>::push(node* new_node) {
    
    // see http://www.boost.org/doc/libs/1_53_0/boost/lockfree/stack.hpp
    #if ORYOL_ALLOCATOR_DEBUG
    o_assert(nullptr == new_node->next);
    o_assert((node_state::init == new_node->state) || (node_state::used == new_node->state));
    memory::fill((void*) (new_node + 1), sizeof(TYPE), 0xAA);
    #endif
    new_node->state = node_state::free;
    new_node->next = head.load(std::memory_order_relaxed);
    for (;;) {
        if (head.compare_exchange_weak(new_node->next, new_node)) {
            break;
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int CHUNK_SIZE>
typename pool_allocator<TYPE,CHUNK_SIZE>::node* pool_allocator<TYPE,CHUNK_SIZE>::pop()
{
    // see http://www.boost.org/doc/libs/1_53_0/boost/lockfree/stack.hpp
    node* old_head = head.load(std::memory_order_consume);
    if (0 == old_head) {
        return nullptr;
    }
    for (;;) {
        if (head.compare_exchange_weak(old_head, old_head->next)) {
            #if ORYOL_ALLOCATOR_DEBUG
            o_assert(node_state::free == old_head->state);
            memory::fill((void*) (old_head + 1), sizeof(TYPE), 0xBB);
            #endif
            old_head->next = nullptr;
            old_head->state = node_state::used;
            return old_head;
        }
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int CHUNK_SIZE>
template<typename... ARGS>
TYPE* pool_allocator<TYPE, CHUNK_SIZE>::create(ARGS&&... args) {
    
    // FIXME: this should take variadic constructor args

    // pop a new node from the free-stack
    node* n;
    while (0 == (n = pop())) {
        alloc_chunk();
    }
    
    // construct with placement new
    void* obj_ptr = (void*) (n + 1);
    TYPE* obj = new(obj_ptr) TYPE(std::forward<ARGS>(args)...);
    o_assert(obj == obj_ptr);
    return obj;
}

//------------------------------------------------------------------------------
template<class TYPE, int CHUNK_SIZE>
bool pool_allocator<TYPE, CHUNK_SIZE>::is_owned(TYPE* obj) const {

    std::lock_guard<std::mutex> lock(chunk_mutex);
    for (void* p : chunks) {
        node* start = (node*) p;
        node* end = start + CHUNK_SIZE * elm_size;
        if (((node*)obj >= start) && ((node*)obj < end)) {
            return true;
        }
    }
    // fallthrough: NOT OWNED!
    return false;
}

//------------------------------------------------------------------------------
template<class TYPE, int CHUNK_SIZE>
void pool_allocator<TYPE, CHUNK_SIZE>::destroy(TYPE* obj) {
    
    #if ORYOL_ALLOCATOR_DEBUG
    // make sure this object has been allocated by us
    o_assert(is_owned(obj));
    #endif
    
    // call destructor on obj
    obj->~TYPE();
    
    // push the chunk element back on the free-stack
    node* n = ((node*)obj) - 1;
    push(n);
}

} // namespace memory
} // namespace oryol