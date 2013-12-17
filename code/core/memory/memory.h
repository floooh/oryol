#pragma once
//------------------------------------------------------------------------------
/**
    @class oryol::memory
    
    Lowlevel memory allocation wrapper for Oryol. Standard memory alignment
    differs by platforms (e.g. platforms with SSE support return 16-byte
    aligned memory.
    
    FIXME: for now this simply calls malloc()/free(), may be more
    advanced in the future.
*/
#include "core/types.h"

namespace oryol {
namespace memory {
    
/// malloc wrapper
inline void* alloc(std::size_t s) {
    return std::malloc(s);
};

/// realloc wrapper
inline void* realloc(void* ptr, std::size_t s) {
    return std::realloc(ptr, s);
};

/// free wrapper
inline void free(void* p) {
    std::free(p);
};

/// copy memory, memory areas must not overlap (NOTE different order of src and dest compared to memcpy!)
inline void copy(void* from, void* to, std::size_t num_bytes) {
    std::memcpy(to, from, num_bytes);
};

/// move memory, memory areas may overlap (NOTE different order of src and dest compared to memmove!)
inline void move(void* from, void* to, std::size_t num_bytes) {
    std::memmove(to, from, num_bytes);
};

/// clear memory
inline void clear(void* ptr, std::size_t num_bytes) {
    std::memset(ptr, 0, num_bytes);
};
    
} // namespace memory
} // namespace oryol