#pragma once
//------------------------------------------------------------------------------
/**
    @class oryol::threading::rwlock
    
    A single-writer / multiple-reader lock. C++11 doesn't provide this
    yet, so we roll our own.
*/
#include "core/types.h"
#include "core/macros.h"

namespace oryol {
namespace threading {

class rwlock {
public:
    /// lock for writing
    void lock_write();
    /// unlock from writing
    void unlock_write();
    /// lock for reading
    void lock_read();
    /// unlock from reading
    void unlock_read();
    
private:
#if ORYOL_HAS_THREADS
    std::atomic<bool> write_lock{false};
    std::atomic<int32> read_count{0};
#endif
};

//------------------------------------------------------------------------------
/**
*/
inline void rwlock::lock_write() {
#if ORYOL_HAS_THREADS
    /// acquire the write-lock
    while (std::atomic_exchange_explicit(&write_lock, true, std::memory_order_acquire)) {
        // spinning...
    }
    /// spin until no-one's reading anymore
    while (read_count > 0) {
        // spinning...
    }
#endif
}

//------------------------------------------------------------------------------
/**
 */
inline void rwlock::unlock_write() {
#if ORYOL_HAS_THREADS
    std::atomic_store_explicit(&write_lock, false, std::memory_order_release);
#endif
}

//------------------------------------------------------------------------------
/**
 */
inline void rwlock::lock_read() {
#if ORYOL_HAS_THREADS
    /// spin until noone's writing anymore
    while (write_lock) {
        // spinning...
    }
    /// increment the read-count
    ++read_count;
#endif
}

//------------------------------------------------------------------------------
/**
 */
inline void rwlock::unlock_read() {
#if ORYOL_HAS_THREADS
    --read_count;
#endif
}

} // namespace threading
} // namespace oryol
