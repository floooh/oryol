#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::stringAtomTable
    
    A thread-local StringAtom table. 
*/
#include "Core/Types.h"
#include "Core/String/stringAtomBuffer.h"
#include "Core/Macros.h"
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

//------------------------------------------------------------------------------
inline int32
stringAtomTable::HashForString(const char* str) {

    // see here: http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
    const char* p = str;
    int32 h = 0;
    char c;
    while (0 != (c = *p++))
    {
        h += c;
        h += (h << 10);
        h ^= (h >> 6);
    }
    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);
    return h;
}

//------------------------------------------------------------------------------
inline bool
stringAtomTable::Entry::operator==(const Entry& rhs) const {

    #if ORYOL_DEBUG
    o_assert(this->header != 0 && rhs.header != 0);
    o_assert(this->header->hash != 0 && rhs.header->hash != 0);
    o_assert(this->header->str != 0 && rhs.header->str != 0);
    #endif
    
    if (this->header->hash != rhs.header->hash) {
        // if hashes differ, the entries are definitely not equal
        return false;
    }
    else {
        // if hashes are identical, need to do a strcmp
        return (0 == std::strcmp(this->header->str, rhs.header->str));
    }
}

//------------------------------------------------------------------------------
inline bool
stringAtomTable::Entry::operator<(const Entry& rhs) const {
        
    #if ORYOL_DEBUG
    o_assert(this->header != 0 && rhs.header != 0);
    o_assert(this->header->hash != 0 && rhs.header->hash != 0);
    o_assert(this->header->str != 0 && rhs.header->str != 0);
    #endif
    return std::strcmp(this->header->str, rhs.header->str) < 0;
}

} // namespace Core
} // namespace Oryol
