//------------------------------------------------------------------------------
//  stringatom_table.cc
//------------------------------------------------------------------------------
#include "pre.h"
#include "stringatom_table.h"

namespace oryol {
namespace string {

oryol_impl_local_singleton(stringatom_table);

//------------------------------------------------------------------------------
stringatom_table::stringatom_table() : table(bucket_count) {
    
    singleton_ensure_unique();
}

//------------------------------------------------------------------------------
const stringatom_buffer::header* stringatom_table::find(std::size_t hash, const char* str) const {
    
    // need to create a temp object for searching in the set
    stringatom_buffer::header dummy_head(this, hash, str);
    entry dummy_entry(&dummy_head);
    
    auto iter = table.find(dummy_entry);
    if (iter == table.end()) {
        // not found
        return nullptr;
    }
    else {
        o_assert(nullptr != iter->header);
        return iter->header;
    }
}

//------------------------------------------------------------------------------
const stringatom_buffer::header* stringatom_table::add(std::size_t hash, const char* str) {
    
    #if ORYOL_DEBUG
    o_assert(nullptr == find(hash, str));
    #endif
    
    // add new string to the string buffer
    const stringatom_buffer::header* new_header = buffer.add_string(this, hash, str);
    o_assert(nullptr != new_header);
    
    // add new entry to our lookup table
    table.emplace(new_header);

    return new_header;
}

} // namespace string
} // namespace oryol

