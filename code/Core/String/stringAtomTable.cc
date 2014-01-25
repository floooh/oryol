//------------------------------------------------------------------------------
//  stringAtomTable.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "stringAtomTable.h"

namespace Oryol {
namespace Core {

OryolLocalSingletonImpl(stringAtomTable);

//------------------------------------------------------------------------------
stringAtomTable::stringAtomTable() {
    
    SingletonEnsureUnique();
}

//------------------------------------------------------------------------------
const stringAtomBuffer::Header*
stringAtomTable::Find(int32 hash, const char* str) const {
    
    // need to create a temp object for searching in the set
    stringAtomBuffer::Header dummyHead(this, hash, str);
    Entry dummyEntry(&dummyHead);
    auto ptr = this->table.Find(dummyEntry);
    if (nullptr == ptr) {
        return nullptr;
    }
    else {
        o_assert(nullptr != ptr->header);
        return ptr->header;
    }
}

//------------------------------------------------------------------------------
const stringAtomBuffer::Header*
stringAtomTable::Add(int32 hash, const char* str) {
    
    #if ORYOL_DEBUG
    o_assert(nullptr == this->Find(hash, str));
    #endif
    
    // add new string to the string buffer
    const stringAtomBuffer::Header* newHeader = this->buffer.AddString(this, hash, str);
    o_assert(nullptr != newHeader);
    
    // add new entry to our lookup table
    this->table.Insert(Entry(newHeader));
    return newHeader;
}

} // namespace Core
} // namespace Oryol


