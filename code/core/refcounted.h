#pragma once
//------------------------------------------------------------------------------
/**
    @class core::refcounted
    
*/
#include "core/types.h"
#include "core/ptr.h"
#include "core/memory/pool_allocator.h"

namespace oryol {
namespace core {
    
class refcounted {
    oryol_class_decl(refcounted, 1024);
public:
    /// destructor
    virtual ~refcounted();
    
    /// add reference
    void add_ref();
    /// release reference (calls destructor when ref_count reaches zero)
    void release();
    /// get reference count
    int32 get_refcount() const;

private:
    #if ORYOL_HAS_THREADS
    std::atomic<int32> ref_count{0};
    #else
    int32 ref_count{0};
    #endif
};

//------------------------------------------------------------------------------
inline void refcounted::add_ref() {
    #if ORYOL_HAS_THREADS
    ref_count.fetch_add(1, std::memory_order_relaxed);
    #else
    ref_count++;
    #endif
}

//------------------------------------------------------------------------------
inline void refcounted::release() {
    #if ORYOL_HAS_THREADS
    if (1 == ref_count.fetch_sub(1, std::memory_order_relaxed)) {
        // destroy() is virtual and provided by the oryol_class_decl macro
        this->destroy();
    }
    #else
    if (1 == ref_count--) {
        delete(this);
    }
    #endif
}

//------------------------------------------------------------------------------
inline int32 refcounted::get_refcount() const
{
    return ref_count;
}

} // namespace core
} // namespace refcounted