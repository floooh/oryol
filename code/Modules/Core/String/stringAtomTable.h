#pragma once
//------------------------------------------------------------------------------
/*
    private class, do not use
    
    A thread-local StringAtom table.
*/
#include "Core/Types.h"
#include "Core/String/stringAtomBuffer.h"
#include "Core/Containers/HashSet.h"
#include "Core/Threading/ThreadLocalPtr.h"

namespace Oryol {

class stringAtomTable {
public:
    /// access to thread-local stringAtomTable (created on demand)
    static stringAtomTable* threadLocalPtr();
    /// compute hash value for string
    static int32_t HashForString(const char* str);
    /// find a matching buffer header in the table
    const stringAtomBuffer::Header* Find(int32_t hash, const char* str) const;
    /// add a string to the atom table
    const stringAtomBuffer::Header* Add(int32_t hash, const char* str);
    
    static ORYOL_THREADLOCAL_PTR(stringAtomTable) ptr;

    /// a bucket entry
    struct Entry {
        /// default constructor
        Entry() : header(0) { };
        /// constructor with header ptr
        Entry(const stringAtomBuffer::Header* h) : header(h) { };
        /// equality operator
        bool operator==(const Entry& rhs) const;
        /// less-then operator
        bool operator<(const Entry& rhs) const;
        
        const stringAtomBuffer::Header* header;
    };
    
    /// hash function for bucket entry
    struct Hasher {
        int32_t operator()(const Entry& e) const {
            return e.header->hash;
        };
    };
    stringAtomBuffer buffer;
    HashSet<Entry, Hasher, 1024> table;
};

} // namespace Oryol
