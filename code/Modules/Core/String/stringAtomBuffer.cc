//------------------------------------------------------------------------------
//  stringAtomBuffer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include <cstring>
#include "stringAtomBuffer.h"
#include "Core/Memory/Memory.h"
#include "Core/Assertion.h"
#if ORYOL_USE_VLD
#include "vld.h"
#endif

namespace Oryol {
    
//------------------------------------------------------------------------------
stringAtomBuffer::~stringAtomBuffer() {
    // release all our allocated chunks
    for (const int8_t* p : this->chunks) {
        Memory::Free((void*)p);
    }
    this->chunks.Clear();
    this->curPointer = 0;
}
    
//------------------------------------------------------------------------------
void
stringAtomBuffer::allocChunk() {
    // need to turn off leak detection for the string atom system, since
    // string atom buffer are never released
    int8_t* newChunk = (int8_t*) Memory::Alloc(this->chunkSize);
    this->chunks.Add(newChunk);
    this->curPointer = newChunk;
}

//------------------------------------------------------------------------------
const stringAtomBuffer::Header*
stringAtomBuffer::AddString(stringAtomTable* table, int32_t hash, const char* str) {
    o_assert(nullptr != table);
    o_assert(nullptr != str);
    
    // no chunks allocated yet?
    if (0 == this->curPointer) {
        this->allocChunk();
    }
    
    // compute length of new entry (header + string len + 0 terminator byte)
    const int strLen = int(std::strlen(str));
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
    head->length = strLen;
    head->str  = (char*) this->curPointer + sizeof(Header);
    #if ORYOL_WINDOWS
    errno_t res = strcpy_s((char*)head->str, strLen + 1, str);
    o_assert(0 == res);
    #else
    std::strcpy((char*)head->str, str);
    #endif

    // set curPointer to the next aligned position
    this->curPointer = (int8_t*) Memory::Align(this->curPointer + requiredSize, sizeof(Header));
    
    return head;
}

} // namespace Oryol
