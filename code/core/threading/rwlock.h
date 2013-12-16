#pragma once
//------------------------------------------------------------------------------
/**
    @class oryol::threading::rwlock

    A single-writer / multiple-reader lock. This isn't provided yet by
    C++11, so we roll our own. This is lock-free, and spins until
    access is allowed, so don't use this for long operations.
*/
#include <atomic>
#include "core/types.h"
#include "core/macros.h"

namespace oryol {
namespace threading {

class rwlock {
public:
    /// constructor
    rwlock() {};
    /// lock for writing
    void lock_write() {
        /// acquire the write-lock
        while (std::atomic_exchange_explicit(&write_lock, true, std::memory_order_acquire)) {
            // spinning...
        }
        /// spin until no-one's reading anymore
        while (read_count > 0) {
            // spinning...
        }
    };
    /// unlock from writing
    void unlock_write() {
        std::atomic_store_explicit(&write_lock, false, std::memory_order_release);
    }
    /// lock for reading
    void lock_read() {
        /// spin until noone's writing anymore
        while (write_lock) {
            // spinning...
        }
        /// increment the read-count
        ++read_count;
    };
    /// unlock from reading
    void unlock_read() {
        --read_count;
        // o_assert(read_count >= 0);
    };

private:
    std::atomic<bool> write_lock{false};
    std::atomic<int32> read_count{0};
};

} // namespace threading
} // namespace oryol
