//------------------------------------------------------------------------------
//  stringAtomTable.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "stringAtomTable.h"

namespace Oryol {
namespace String {

OryolLocalSingletonImpl(stringAtomTable);

//------------------------------------------------------------------------------
stringAtomTable::stringAtomTable() : table(bucketCount) {
    
    SingletonEnsureUnique();
}

//------------------------------------------------------------------------------
const stringAtomBuffer::Header*
stringAtomTable::Find(std::size_t hash, const char* str) const {
    
    // need to create a temp object for searching in the set
    stringAtomBuffer::Header dummyHead(this, hash, str);
    Entry dummyEntry(&dummyHead);
    
    auto iter = this->table.find(dummyEntry);
    if (iter == this->table.end()) {
        // not found
        return nullptr;
    }
    else {
        o_assert(nullptr != iter->header);
        return iter->header;
    }
}

//------------------------------------------------------------------------------
const stringAtomBuffer::Header*
stringAtomTable::Add(std::size_t hash, const char* str) {
    
    #if ORYOL_DEBUG
    o_assert(nullptr == this->Find(hash, str));
    #endif
    
    // add new string to the string buffer
    const stringAtomBuffer::Header* newHeader = buffer.AddString(this, hash, str);
    o_assert(nullptr != newHeader);
    
    // add new entry to our lookup table
    this->table.emplace(newHeader);

    return newHeader;
}

} // namespace String
} // namespace Oryol


