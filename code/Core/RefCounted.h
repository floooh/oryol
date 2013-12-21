#pragma once
//------------------------------------------------------------------------------
/**
    @class Core::RefCounted
    
    Reference-counted base class for use together with the Ptr<> smart-
    pointer class.
*/
#include "Core/Types.h"
#include "Core/Ptr.h"
#include "Core/Memory/poolAllocator.h"

namespace Oryol {
namespace Core {
    
class RefCounted {
    OryolClassDecl(RefCounted);
public:
    /// destructor
    virtual ~RefCounted();
    
    /// get reference count
    int32 GetRefCount() const;

    /// add reference
    void addRef();
    /// release reference (calls destructor when ref_count reaches zero)
    void release();

private:
    #if ORYOL_HAS_THREADS
    std::atomic<int32> refCount{0};
    #else
    int32 refCount{0};
    #endif
};

//------------------------------------------------------------------------------
inline void
RefCounted::addRef() {
    #if ORYOL_HAS_THREADS
    this->refCount.fetch_add(1, std::memory_order_relaxed);
    #else
    this->refCount++;
    #endif
}

//------------------------------------------------------------------------------
inline void
RefCounted::release() {
    #if ORYOL_HAS_THREADS
    if (1 == this->refCount.fetch_sub(1, std::memory_order_relaxed)) {
        // destroy() is virtual and provided by the oryol_class_decl macro
        this->destroy();
    }
    #else
    if (1 == this->refCount--) {
        delete(this);
    }
    #endif
}

//------------------------------------------------------------------------------
inline int32
RefCounted::GetRefCount() const
{
    return this->refCount;
}

} // namespace Core
} // namespace Oryol