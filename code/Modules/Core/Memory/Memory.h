#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Memory
    @brief Low-level memory management functions
    
    Lowlevel memory allocation wrapper for Oryol. Standard memory alignment
    differs by platforms (e.g. platforms with SSE support return 16-byte
    aligned memory.
    
    @todo for now this simply calls malloc()/free(), may be more
    advanced in the future.
*/
#include "Core/Types.h"
#include "Core/Config.h"

namespace Oryol {
namespace Core {
    
class Memory {
public:
    /// allocate a raw chunk of memory
    static void* Alloc(int32 numBytes);
    /// re-allocate a raw chunk of memory
    static void* ReAlloc(void* ptr, int32 numBytes);
    /// free a raw chunk of memory
    static void Free(void* ptr);
    /// fill range of memory with a byte value
    static void Fill(void* ptr, int32 numBytes, uint8 value);
    /// copy a raw chunk of non-overlapping memory
    static void Copy(const void* from, void* to, int32 numBytes);
    /// move a raw chunk of potentially overlapping memory
    static void Move(const void* from, void* to, int32 numBytes);
    /// fill a chunk of memory with zeros
    static void Clear(void* ptr, int32 numBytes);
    /// align a pointer to size up to ORYOL_MAX_PLATFORM_ALIGN
    static void* Align(void* ptr, int32 byteSize);
    /// round-up a value to the next multiple of byteSize
    static int32 RoundUp(int32 val, int32 byteSize);
};

//------------------------------------------------------------------------------
inline void*
Memory::Align(void* ptr, int32 byteSize) {
    intptr align = byteSize > ORYOL_MAX_PLATFORM_ALIGN ? ORYOL_MAX_PLATFORM_ALIGN : byteSize;
    intptr ptri = (intptr)ptr;
    ptri = (ptri + (align - 1)) & ~(align - 1);
    return (void*) ptri;
};

//------------------------------------------------------------------------------
inline int32
Memory::RoundUp(int32 val, int32 byteSize) {
    return (val + (byteSize - 1)) & ~(byteSize - 1);
}
    
} // namespace Core
} // namespace oryol