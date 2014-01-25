//------------------------------------------------------------------------------
//  String.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "String.h"
#include "StringAtom.h"

namespace Oryol {
namespace Core {

const char* String::emptyString = "";

//------------------------------------------------------------------------------
String::String(const StringAtom& str) {
    const char* cstr = str.AsCStr();
    this->create(cstr, std::strlen(cstr));
}
    
//------------------------------------------------------------------------------
void
String::operator=(const StringAtom& str) {
    this->release();
    const char* cstr = str.AsCStr();
    this->create(cstr, std::strlen(cstr));
}

//------------------------------------------------------------------------------
StringAtom
String::AsStringAtom() const {
    return StringAtom(this->AsCStr());;
}

//------------------------------------------------------------------------------
void
String::destroy() {
    o_assert(0 != this->data);
    o_assert(0 == this->data->refCount);
    this->data->~StringData();
    Memory::Free(this->data);
    this->data = 0;
    this->strPtr = 0;
}

//------------------------------------------------------------------------------
void
String::alloc(int32 len) {
    o_assert(len > 0);
    this->data = (StringData*) Memory::Alloc(sizeof(StringData) + len + 1);
    new(this->data) StringData();
    this->addRef();
    this->data->length = len;
    this->strPtr = (const char*) &(this->data[1]);
}

//------------------------------------------------------------------------------
void
String::create(const char* ptr, int32 len) {
    o_assert(0 != ptr);
    if ((ptr[0] != 0) && (len > 0)) {
        this->alloc(len);
        Memory::Copy(ptr, (void*)this->strPtr, len);
        ((char*)this->strPtr)[len] = 0;
    }
    else {
        // empty string, don't bother to allocate storage for this
        this->data = nullptr;
        this->strPtr = nullptr;
    }
}

//------------------------------------------------------------------------------
char*
String::Alloc(int32 numCharBytes) {
    this->release();
    this->alloc(numCharBytes);
    Memory::Clear((void*)this->strPtr, numCharBytes + 1);
    return (char*)this->strPtr;
}

//------------------------------------------------------------------------------
bool operator==(const String& s0, const StringAtom& s1) {
    return std::strcmp(s0.AsCStr(), s1.AsCStr()) == 0;
}

//------------------------------------------------------------------------------
bool operator!=(const String& s0, const StringAtom& s1) {
    return std::strcmp(s0.AsCStr(), s1.AsCStr()) != 0;
}

//------------------------------------------------------------------------------
bool operator<(const String& s0, const StringAtom& s1) {
    return std::strcmp(s0.AsCStr(), s1.AsCStr()) < 0;
}

//------------------------------------------------------------------------------
bool operator>(const String& s0, const StringAtom& s1) {
    return std::strcmp(s0.AsCStr(), s1.AsCStr()) > 0;
}

//------------------------------------------------------------------------------
bool operator<=(const String& s0, const StringAtom& s1) {
    return std::strcmp(s0.AsCStr(), s1.AsCStr()) <= 0;
}

//------------------------------------------------------------------------------
bool operator>=(const String& s0, const StringAtom& s1) {
    return std::strcmp(s0.AsCStr(), s1.AsCStr()) >= 0;
}

//------------------------------------------------------------------------------
bool operator==(const StringAtom& s0, const String& s1) {
    return std::strcmp(s0.AsCStr(), s1.AsCStr()) == 0;
}

//------------------------------------------------------------------------------
bool operator!=(const StringAtom& s0, const String& s1) {
    return std::strcmp(s0.AsCStr(), s1.AsCStr()) != 0;
}

//------------------------------------------------------------------------------
bool operator<(const StringAtom& s0, const String& s1) {
    return std::strcmp(s0.AsCStr(), s1.AsCStr()) < 0;
}

//------------------------------------------------------------------------------
bool operator>(const StringAtom& s0, const String& s1) {
    return std::strcmp(s0.AsCStr(), s1.AsCStr()) > 0;
}

//------------------------------------------------------------------------------
bool operator<=(const StringAtom& s0, const String& s1) {
    return std::strcmp(s0.AsCStr(), s1.AsCStr()) <= 0;
}

//------------------------------------------------------------------------------
bool operator>=(const StringAtom& s0, const String& s1) {
    return std::strcmp(s0.AsCStr(), s1.AsCStr()) >= 0;
}

} // namespace Core
} // namespace Oryol