//------------------------------------------------------------------------------
//  Memory.cc
//------------------------------------------------------------------------------
#include <memory>
#include <cstdlib>
#include <cstring>
#include "Memory.h"

namespace Oryol {
namespace Core {
    
//------------------------------------------------------------------------------
void*
Memory::Alloc(int32 numBytes) {
    void* ptr = std::malloc(numBytes);
#if ORYOL_ALLOCATOR_DEBUG || ORYOL_UNITTESTS
    Memory::Fill(ptr, numBytes, ORYOL_MEMORY_DEBUG_BYTE);
#endif
    return ptr;
}

//------------------------------------------------------------------------------
void
Memory::Fill(void* ptr, int32 numBytes, uint8 value) {
    std::memset(ptr, value, numBytes);
}

//------------------------------------------------------------------------------
void*
Memory::ReAlloc(void* ptr, int32 s) {
    // @fixme: HMM need to fix fill with debug pattern...
    return std::realloc(ptr, s);
}

//------------------------------------------------------------------------------
void
Memory::Free(void* p) {
    std::free(p);
}

//------------------------------------------------------------------------------
void
Memory::Copy(const void* from, void* to, int32 numBytes) {
    std::memcpy(to, from, numBytes);
}

//------------------------------------------------------------------------------
void
Memory::Move(const void* from, void* to, int32 numBytes) {
    std::memmove(to, from, numBytes);
}

//------------------------------------------------------------------------------
void
Memory::Clear(void* ptr, int32 numBytes) {
    std::memset(ptr, 0, numBytes);
}

} // namespace Core
} // namespace Oryol



