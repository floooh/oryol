#pragma once
//------------------------------------------------------------------------------
/*
    private class, do not use
    
    A growable buffer for raw string data for the StringAtom system.
*/
#include "Core/Types.h"
#include "Core/Containers/Array.h"

namespace Oryol {
namespace Core {

class stringAtomTable;

class stringAtomBuffer {
public:
    // header data for a single entry (string data starts at end of header)
    struct Header {
        // default constructor
        Header() : table(0), hash(0), length(0), str(0) { };
        /// constructor
        Header(const stringAtomTable* t, int32 hsh, int32 len, const char* s) : table(t), hash(hsh), length(len), str(s) { };
    
        const stringAtomTable* table;
        int32 hash;
        int32 length;
        const char* str;
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
