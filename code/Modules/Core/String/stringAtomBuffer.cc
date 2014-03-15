//------------------------------------------------------------------------------
//  stringAtomBuffer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include <cstring>
#include "stringAtomBuffer.h"
#include "Core/Memory/Memory.h"
#include "Core/Assert.h"

namespace Oryol {
namespace Core {
    
//------------------------------------------------------------------------------
stringAtomBuffer::~stringAtomBuffer() {
    // release all our allocated chunks
    for (const int8* p : this->chunks) {
        Memory::Free((void*)p);
    }
    this->chunks.Clear();
    this->curPointer = 0;
}
    
//------------------------------------------------------------------------------
void
stringAtomBuffer::allocChunk() {
    int8* newChunk = (int8*) Memory::Alloc(this->chunkSize);
    this->chunks.AddBack(newChunk);
    this->curPointer = newChunk;
}

//------------------------------------------------------------------------------
const stringAtomBuffer::Header*
stringAtomBuffer::AddString(stringAtomTable* table, int32 hash, const char* str) {
    o_assert(nullptr != table);
    o_assert(nullptr != str);
    
    // no chunks allocated yet?
    if (0 == this->curPointer) {
        this->allocChunk();
    }
    
    // compute length of new entry (header + string len + 0 terminator byte)
    const int32 strLen = std::strlen(str);
    size_t requiredSize = strLen + sizeof(Header) + 1;
    o_assert(requiredSize < this->chunkSize);
    
    // check if there's enough room in the current chunk
    if ((this->curPointer + requiredSize) >= (this->chunks.Back() + this->chunkSize)) {
        this->allocChunk();
    }
    
    // copy over data
    Header* head = (Header*) this->curPointer;
    head->table = table;
    head->hash = hash;
    head->length  = strLen;
    head->str  = (char*) this->curPointer + sizeof(Header);
    std::strcpy((char*)head->str, str);
    
    // set curPointer to the next aligned position
    this->curPointer = (int8*) Memory::Align(this->curPointer + requiredSize, sizeof(Header));
    
    return head;
}

} // namespace Core
} // namespace Oryol
