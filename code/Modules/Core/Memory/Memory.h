#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Memory
    @ingroup Core
    @brief Low-level memory management functions
    
    Lowlevel memory allocation wrapper for Oryol. Standard memory alignment
    differs by platforms (e.g. platforms with SSE support return 16-byte
    aligned memory.
    
    @todo for now this simply calls malloc()/free(), may be more
    advanced in the future.
*/
#include "Core/Types.h"
#include "Core/Config.h"
#include <new>
#include <utility>

namespace Oryol {
    
class Memory {
public:
    /// allocate a raw chunk of memory
    static void* Alloc(int numBytes);
    /// re-allocate a raw chunk of memory
    static void* ReAlloc(void* ptr, int numBytes);
    /// free a raw chunk of memory
    static void Free(void* ptr);
    /// fill range of memory with a byte value
    static void Fill(void* ptr, int numBytes, uint8_t value);
    /// copy a raw chunk of non-overlapping memory
    static void Copy(const void* from, void* to, int numBytes);
    /// move a raw chunk of potentially overlapping memory
    static void Move(const void* from, void* to, int numBytes);
    /// fill a chunk of memory with zeros
    static void Clear(void* ptr, int numBytes);
    /// align a pointer to size up to ORYOL_MAX_PLATFORM_ALIGN
    static void* Align(void* ptr, int byteSize);
    /// round-up a value to the next multiple of byteSize
    static int RoundUp(int val, int byteSize);
    /// replacement for new() going through Memory::Alloc without overriding new
    template<class TYPE, typename... ARGS> static TYPE* New(ARGS&&... args) {
        TYPE* ptr = (TYPE*) Memory::Alloc(sizeof(TYPE));
        return new(ptr) TYPE(std::forward<ARGS>(args)...);
    };
    /// replacement delete (see Memory::New())
    template<class TYPE> static void Delete(TYPE* ptr) {
        ptr->~TYPE();
        Memory::Free(ptr);
    };
};

//------------------------------------------------------------------------------
inline void*
Memory::Align(void* ptr, int byteSize) {
    intptr_t align = byteSize > ORYOL_MAX_PLATFORM_ALIGN ? ORYOL_MAX_PLATFORM_ALIGN : byteSize;
    intptr_t ptri = (intptr_t)ptr;
    ptri = (ptri + (align - 1)) & ~(align - 1);
    return (void*) ptri;
};

//------------------------------------------------------------------------------
inline int
Memory::RoundUp(int val, int roundTo) {
    return (val + (roundTo - 1)) & ~(roundTo - 1);
}
    
} // namespace oryol