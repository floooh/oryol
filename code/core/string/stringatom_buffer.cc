//------------------------------------------------------------------------------
//  stringatom_buffer.cc
//------------------------------------------------------------------------------
#include "pre.h"
#include "stringatom_buffer.h"
#include "core/memory/memory.h"
#include "core/macros.h"

namespace oryol {
namespace string {
    
//------------------------------------------------------------------------------
stringatom_buffer::~stringatom_buffer() {
    // release all our allocated chunks
    for (const int8* p : chunks) {
        memory::free((void*)p);
    }
    chunks.clear();
    cur_pointer = 0;
}
    
//------------------------------------------------------------------------------
void stringatom_buffer::alloc_chunk() {
    int8* new_chunk = (int8*) memory::alloc(chunk_size);
    chunks.push_back(new_chunk);
    cur_pointer = new_chunk;
}

//------------------------------------------------------------------------------
const stringatom_buffer::header* stringatom_buffer::add_string(stringatom_table* table, std::size_t hash, const char* str) {
    o_assert(table);
    o_assert(str);
    
    // no chunks allocated yet?
    if (0 == cur_pointer) {
        alloc_chunk();
    }
    
    // compute length of new entry (header + string len + 0 terminator byte)
    size_t required_size = std::strlen(str) + sizeof(header) + 1;
    o_assert(required_size < chunk_size);
    
    // check if there's enough room in the current chunk
    if ((cur_pointer + required_size) >= (chunks.back() + chunk_size)) {
        this->alloc_chunk();
    }
    
    // copy over data
    header* head = (header*) cur_pointer;
    head->table = table;
    head->hash = hash;
    head->str  = (char*) cur_pointer + sizeof(header);
    std::strcpy((char*)head->str, str);
    
    // set cur_pointer to the next aligned position
    cur_pointer = (int8*) memory::align(cur_pointer + required_size, sizeof(header));
    
    return head;
}

} // namespace string
} // namespace oryol