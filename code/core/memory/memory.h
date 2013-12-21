#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Memory
    
    Lowlevel memory allocation wrapper for Oryol. Standard memory alignment
    differs by platforms (e.g. platforms with SSE support return 16-byte
    aligned memory.
    
    FIXME: for now this simply calls malloc()/free(), may be more
    advanced in the future.
*/
#include "Core/Types.h"

namespace Oryol {
namespace Memory {
    
//------------------------------------------------------------------------------
inline void*
Alloc(std::size_t s) {
    return std::malloc(s);
};

//------------------------------------------------------------------------------
inline void*
ReAlloc(void* ptr, std::size_t s) {
    return std::realloc(ptr, s);
};

//------------------------------------------------------------------------------
inline void
Free(void* p) {
    std::free(p);
};

//------------------------------------------------------------------------------
inline void
Copy(void* from, void* to, std::size_t num_bytes) {
    // copy memory, memory areas must not overlap
    // (NOTE different order of src and dest compared to memcpy!)
    std::memcpy(to, from, num_bytes);
};

//------------------------------------------------------------------------------
inline void
Move(void* from, void* to, std::size_t num_bytes) {
    // move memory, memory areas may overlap
    // (NOTE different order of src and dest compared to memmove!)
    std::memmove(to, from, num_bytes);
};

//------------------------------------------------------------------------------
inline void
Clear(void* ptr, std::size_t num_bytes) {
    std::memset(ptr, 0, num_bytes);
};

//------------------------------------------------------------------------------
inline void
Fill(void* ptr, std::size_t num_bytes, uint8 value) {
    std::memset(ptr, value, num_bytes);
}

//------------------------------------------------------------------------------
inline void*
Align(void* ptr, uint32 byte_size) {
    // align a pointer to a byte size or a platform-specific maximum byte size
    uintptr align = byte_size > ORYOL_MAX_PLATFORM_ALIGN ? ORYOL_MAX_PLATFORM_ALIGN : byte_size;
    uintptr ptri = (uintptr)ptr;
    ptri = (ptri + (align - 1)) & ~(align - 1);
    return (void*) ptri;
};

//------------------------------------------------------------------------------
inline uint32
RoundUp(uint32 val, uint32 byte_size) {
    return (val + (byte_size - 1)) & ~(byte_size - 1);
}
    
} // namespace memory
} // namespace oryol