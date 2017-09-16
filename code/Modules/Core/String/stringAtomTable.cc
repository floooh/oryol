//------------------------------------------------------------------------------
//  stringAtomTable.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include <cstring>
#include "stringAtomTable.h"
#if ORYOL_USE_VLD
#include "vld.h"
#endif

namespace Oryol {

ORYOL_THREADLOCAL_PTR(stringAtomTable) stringAtomTable::ptr = nullptr;

//------------------------------------------------------------------------------
stringAtomTable*
stringAtomTable::threadLocalPtr() {
    // NOTE: this object can never be released, even if a thread is left
    // since StringAtom object can move to other threads, thus memory
    // leak detectors will complain about these allocations on program
    // exit
    if (!ptr) {
        #if ORYOL_USE_VLD
        VLDDisable();
        #endif
        ptr = Memory::New<stringAtomTable>();
        #if ORYOL_USE_VLD
        VLDEnable();
        #endif
    }
    return ptr;
}

//------------------------------------------------------------------------------
const stringAtomBuffer::Header*
stringAtomTable::Find(int32_t hash, const char* str) const {
    
    // need to create a temp object for searching in the set
    stringAtomBuffer::Header dummyHead(this, hash, 0, str);
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
stringAtomTable::Add(int32_t hash, const char* str) {
    
    #if ORYOL_DEBUG
    o_assert(nullptr == this->Find(hash, str));
    #endif    
    #if ORYOL_USE_VLD
    VLDDisable();
    #endif

    // add new string to the string buffer
    const stringAtomBuffer::Header* newHeader = this->buffer.AddString(this, hash, str);
    o_assert(nullptr != newHeader);
    
    // add new entry to our lookup table
    this->table.Add(Entry(newHeader));

    #if ORYOL_USE_VLD
    VLDEnable();
    #endif
    return newHeader;
}

//------------------------------------------------------------------------------
int32_t
stringAtomTable::HashForString(const char* str) {

    // see here: http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
    const char* p = str;
    uint32_t h = 0;
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
bool
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
bool
stringAtomTable::Entry::operator<(const Entry& rhs) const {
        
    #if ORYOL_DEBUG
    o_assert(this->header != 0 && rhs.header != 0);
    o_assert(this->header->hash != 0 && rhs.header->hash != 0);
    o_assert(this->header->str != 0 && rhs.header->str != 0);
    #endif
    return std::strcmp(this->header->str, rhs.header->str) < 0;
}

} // namespace Oryol


