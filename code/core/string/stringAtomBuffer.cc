//------------------------------------------------------------------------------
//  stringAtomBuffer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "stringAtomBuffer.h"
#include "Core/Memory/Memory.h"
#include "Core/Macros.h"

namespace Oryol {
namespace String {
    
//------------------------------------------------------------------------------
stringAtomBuffer::~stringAtomBuffer() {
    // release all our allocated chunks
    for (const int8* p : this->chunks) {
        Memory::Free((void*)p);
    }
    this->chunks.clear();
    this->curPointer = 0;
}
    
//------------------------------------------------------------------------------
void
stringAtomBuffer::allocChunk() {
    int8* newChunk = (int8*) Memory::Alloc(this->chunkSize);
    this->chunks.push_back(newChunk);
    this->curPointer = newChunk;
}

//------------------------------------------------------------------------------
const stringAtomBuffer::Header*
stringAtomBuffer::AddString(stringAtomTable* table, std::size_t hash, const char* str) {
    o_assert(table);
    o_assert(str);
    
    // no chunks allocated yet?
    if (0 == this->curPointer) {
        this->allocChunk();
    }
    
    // compute length of new entry (header + string len + 0 terminator byte)
    size_t requiredSize = std::strlen(str) + sizeof(Header) + 1;
    o_assert(requiredSize < this->chunkSize);
    
    // check if there's enough room in the current chunk
    if ((this->curPointer + requiredSize) >= (this->chunks.back() + this->chunkSize)) {
        this->allocChunk();
    }
    
    // copy over data
    Header* head = (Header*) this->curPointer;
    head->table = table;
    head->hash = hash;
    head->str  = (char*) this->curPointer + sizeof(Header);
    std::strcpy((char*)head->str, str);
    
    // set curPointer to the next aligned position
    this->curPointer = (int8*) Memory::Align(this->curPointer + requiredSize, sizeof(Header));
    
    return head;
}

} // namespace String
} // namespace Oryol
