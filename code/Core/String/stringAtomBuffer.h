#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::stringAtomBuffer
  
    A growable buffer for raw string data for the StringAtom system.
*/
#include "Core/Types.h"
#include "Core/Containers/Array.h"

namespace Oryol {
namespace Core {

class stringAtomTable;

class stringAtomBuffer {
public:
    /// header data for a single entry (string data starts at end of header)
    struct Header {
        // default constructor
        Header() : table(0), hash(0), str(0) { };
        /// constructor
        Header(const stringAtomTable* t, int32 h, const char* s) : table(t), hash(h), str(s) { };
    
        const stringAtomTable* table = 0;
        int32 hash = 0;
        const char* str = 0;
    };

    /// destructor
    ~stringAtomBuffer();
    
    /// add a new string to the buffer, return pointer to start of header
    const Header* AddString(stringAtomTable* table, int32 hash, const char* str);
    
private:
    /// allocate a new chunk
    void allocChunk();

    static const int32 chunkSize = (1<<14);    // careful with this: each thread has its own stringbuffer!
    Array<int8*> chunks;
    int8* curPointer = 0;        // this is always aligned to min(sizeof(header), ORYOL_MAX_PLATFORM_ALIGN)
};
    
} // namespace Core
} // namespace Oryol
