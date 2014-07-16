#pragma once
//------------------------------------------------------------------------------
/*
    private class, do not use
    
    A thread-local StringAtom table.
*/
#include "Core/Types.h"
#include "Core/String/stringAtomBuffer.h"
#include "Core/Singleton.h"
#include "Core/Containers/HashSet.h"

namespace Oryol {
namespace Core {

class stringAtomTable {
    OryolLocalSingletonDecl(stringAtomTable);
public:
    /// constructor
    stringAtomTable();
    /// destructor
    ~stringAtomTable();
    
    /// compute hash value for string
    static int32 HashForString(const char* str);
    /// find a matching buffer header in the table
    const stringAtomBuffer::Header* Find(int32 hash, const char* str) const;
    /// add a string to the atom table
    const stringAtomBuffer::Header* Add(int32 hash, const char* str);

private:
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
        int32 operator()(const Entry& e) const {
            return e.header->hash;
        };
    };
    stringAtomBuffer buffer;
    Core::HashSet<Entry, Hasher, 1024> table;
};

} // namespace Core
} // namespace Oryol
