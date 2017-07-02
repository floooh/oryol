#pragma once
//------------------------------------------------------------------------------
/*
    private class, do not use
    
    A growable buffer for raw string data for the StringAtom system.
*/
#include "Core/Types.h"
#include "Core/Containers/Array.h"

namespace Oryol {

class stringAtomTable;

class stringAtomBuffer {
public:
    // header data for a single entry (string data starts at end of header)
    struct Header {
        // default constructor
        Header() : table(0), hash(0), length(0), str(0) { };
        /// constructor
        Header(const stringAtomTable* t, int32_t hsh, int len, const char* s) : table(t), hash(hsh), length(len), str(s) { };
    
        const stringAtomTable* table;
        int32_t hash;
        int length;
        const char* str;
    };

    /// destructor
    ~stringAtomBuffer();
    /// add a new string to the buffer, return pointer to start of header
    const Header* AddString(stringAtomTable* table, int32_t hash, const char* str);
    /// allocate a new chunk
    void allocChunk();

    static const int chunkSize = (1<<14);    // careful with this: each thread has its own stringbuffer!
    Array<int8_t*> chunks;
    int8_t* curPointer = 0;        // this is always aligned to min(sizeof(header), ORYOL_MAX_PLATFORM_ALIGN)
};
    
} // namespace Oryol
