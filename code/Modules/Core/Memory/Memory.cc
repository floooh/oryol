//------------------------------------------------------------------------------
//  Memory.cc
//------------------------------------------------------------------------------
#include <memory>
#include <cstdlib>
#include <cstring>
#include "Memory.h"
#if ORYOL_USE_VLD
#include "vld.h"
#endif

namespace Oryol {
    
//------------------------------------------------------------------------------
void*
Memory::Alloc(int numBytes) {
    void* ptr = std::malloc(numBytes);
#if ORYOL_ALLOCATOR_DEBUG || ORYOL_UNITTESTS
    Memory::Fill(ptr, numBytes, ORYOL_MEMORY_DEBUG_BYTE);
#endif
    return ptr;
}

//------------------------------------------------------------------------------
void
Memory::Fill(void* ptr, int numBytes, uint8_t value) {
    std::memset(ptr, value, numBytes);
}

//------------------------------------------------------------------------------
void*
Memory::ReAlloc(void* ptr, int s) {
    /// @todo: HMM need to fix fill with debug pattern...
    return std::realloc(ptr, s);
}

//------------------------------------------------------------------------------
void
Memory::Free(void* p) {
    std::free(p);
}

//------------------------------------------------------------------------------
void
Memory::Copy(const void* from, void* to, int numBytes) {
    std::memcpy(to, from, numBytes);
}

//------------------------------------------------------------------------------
void
Memory::Move(const void* from, void* to, int numBytes) {
    std::memmove(to, from, numBytes);
}

//------------------------------------------------------------------------------
void
Memory::Clear(void* ptr, int numBytes) {
    std::memset(ptr, 0, numBytes);
}

} // namespace Oryol



