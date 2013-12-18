#pragma once
//------------------------------------------------------------------------------
/**
    @class oryol::string::stringatom_table
    
    A thread-local stringatom table. 
*/
#include "core/types.h"
#include "core/string/stringatom_buffer.h"
#include "core/macros.h"
#include <unordered_set>

namespace oryol {
namespace string {

class stringatom_table {
    oryol_decl_local_singleton(stringatom_table);
public:
    /// constructor
    stringatom_table();
    /// destructor
    ~stringatom_table();
    
    /// compute hash value for string
    static std::size_t hash_for_string(const char* str);
    /// find a matching buffer header in the table
    const stringatom_buffer::header* find(std::size_t hash, const char* str) const;
    /// add a string to the atom table
    const stringatom_buffer::header* add(std::size_t hash, const char* str);
    /// debug: get all entries in the table
    std::vector<stringatom_buffer::header*> dbg_get_entries() const;

    /// a bucket entry
    struct entry {
        /// default constructor
        entry() : header(0) { };
        /// constructor with header ptr
        entry(const stringatom_buffer::header* h) : header(h) { };
        /// equality operator
        bool operator==(const entry& rhs) const;
        
        const stringatom_buffer::header* header;
    };
    
    /// hash function for bucket entry
    struct hash {
        std::size_t operator()(const entry& e) const {
            return e.header->hash;
        };
    };
    static const std::size_t bucket_count = 256;
    stringatom_buffer buffer;
    std::unordered_set<entry, hash> table;
};

//------------------------------------------------------------------------------
inline std::size_t stringatom_table::hash_for_string(const char* str) {

    // see here: http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
    const char* p = str;
    std::size_t h = 0;
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
inline bool stringatom_table::entry::operator==(const entry& rhs) const {

    #if ORYOL_DEBUG
    o_assert(header != 0 && rhs.header != 0);
    o_assert(header->hash != 0 && rhs.header->hash != 0);
    o_assert(header->str != 0 && rhs.header->str != 0);
    #endif
    
    if (header->hash != rhs.header->hash) {
        // if hashes differ, the entries are definitely not equal
        return false;
    }
    else {
        // if hashes are identical, need to do a strcmp
        return (0 == std::strcmp(header->str, rhs.header->str));
    }
}

} // namespace string
} // namespace oryol