#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::RefCounted
    @ingroup Core
    @brief Oryol's reference-counted base class

    The RefCounted class is used together with the Ptr smart-pointer class
    to automatically manage the life-time of objects through 
    reference-counting.
 
    @see Ptr
*/
#include "Core/Types.h"
#include "Core/Ptr.h"
#include "Core/Class.h"
#include "Core/Config.h"
#if ORYOL_HAS_ATOMIC
#include <atomic>
#endif

namespace Oryol {
    
class RefCounted {
    OryolBaseClassDecl(RefCounted);
public:
    /// destructor needs to be virtual
    virtual ~RefCounted() { };
    /// get reference count
    int GetRefCount() const;
    /// add reference
    void addRef();
    /// release reference (calls destructor when ref_count reaches zero)
    void release();

private:
    #if ORYOL_HAS_ATOMIC
    std::atomic<int> refCount{0};
    #else
    int32_t refCount{0};
    #endif
};

//------------------------------------------------------------------------------
inline void
RefCounted::addRef() {
    #if ORYOL_HAS_ATOMIC
    this->refCount.fetch_add(1, std::memory_order_relaxed);
    #else
    this->refCount++;
    #endif
}

//------------------------------------------------------------------------------
inline void
RefCounted::release() {
    #if ORYOL_HAS_ATOMIC
    if (1 == this->refCount.fetch_sub(1, std::memory_order_relaxed)) {
    #else
    if (1 == this->refCount--) {
    #endif
        // destroy() is virtual and provided by the OryolClassDecl macro
        this->destroy();
    }
}

//------------------------------------------------------------------------------
inline int
RefCounted::GetRefCount() const
{
    return this->refCount;
}

} // namespace Oryol
