#pragma once
//------------------------------------------------------------------------------
/**
    @class string::stringatom_buffer
  
    A growable buffer for raw string data for the stringatom system.
*/
#include "core/types.h"

namespace oryol {
namespace string {

class stringatom_table;

class stringatom_buffer {
public:
    /// header data for a single entry (string data starts at end of header)
    struct header {
        // default constructor
        header() : table(0), hash(0), str(0) { };
        /// constructor
        header(const stringatom_table* t, std::size_t h, const char* s) : table(t), hash(h), str(s) { };
    
        const stringatom_table* table = 0;
        std::size_t hash = 0;
        const char* str = 0;
    };

    /// destructor
    ~stringatom_buffer();
    
    /// add a new string to the buffer, return pointer to start of header
    const header* add_string(stringatom_table* table, std::size_t hash, const char* str);
    
private:
    /// allocate a new chunk
    void alloc_chunk();

    static const int32 chunk_size = (1<<14);    // careful with this: each thread has its own stringbuffer!
    std::vector<int8*> chunks;
    int8* cur_pointer = 0;        // this is always aligned to min(sizeof(header), ORYOL_MAX_PLATFORM_ALIGN)
};
    
} // namespace string
} // namespace oryol