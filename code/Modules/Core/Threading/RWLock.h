#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::RWLock
    @brief single-write / multiple-reader lock
*/
#include "Core/Types.h"

namespace Oryol {
namespace Core {

class RWLock {
public:
    /// lock for writing
    void LockWrite();
    /// unlock from writing
    void UnlockWrite();
    /// lock for reading
    void LockRead();
    /// unlock from reading
    void UnlockRead();
    
private:
#if ORYOL_HAS_THREADS
    std::atomic<bool> writeLock{false};
    std::atomic<int32> readCount{0};
#endif
};

//------------------------------------------------------------------------------
inline void
RWLock::LockWrite() {
#if ORYOL_HAS_THREADS
    /// acquire the write-lock
    while (std::atomic_exchange_explicit(&this->writeLock, true, std::memory_order_acquire)) {
        // spinning...
    }
    /// spin until no-one's reading anymore
    while (this->readCount > 0) {
        // spinning...
    }
#endif
}

//------------------------------------------------------------------------------
inline void
RWLock::UnlockWrite() {
#if ORYOL_HAS_THREADS
    std::atomic_store_explicit(&this->writeLock, false, std::memory_order_release);
#endif
}

//------------------------------------------------------------------------------
inline void
RWLock::LockRead() {
#if ORYOL_HAS_THREADS
    /// spin until noone's writing anymore
    while (this->writeLock) {
        // spinning...
    }
    /// increment the read-count
    ++this->readCount;
#endif
}

//------------------------------------------------------------------------------
inline void
RWLock::UnlockRead() {
#if ORYOL_HAS_THREADS
    --this->readCount;
#endif
}

} // namespace Core
} // namespace Oryol

