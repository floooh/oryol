//------------------------------------------------------------------------------
//  String.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include <cstring>
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
String::String(const char* str) {
    if (nullptr != str) {
        this->create(str, std::strlen(str));
    }
    else {
        this->data = nullptr;
        this->strPtr = nullptr;
    }
}

//------------------------------------------------------------------------------
String::String() :
data(nullptr),
strPtr(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
/**
 Construct string from substring of other string. If len is 0, this
 means "until end of string".
*/
String::String(const String& rhs, int32 startIndex, int32 endIndex) {
    if (EndOfString == endIndex) {
        endIndex = rhs.Length();
    }
    o_assert((startIndex >= 0) && (startIndex < endIndex));
    o_assert(endIndex <= rhs.Length());
    this->create(rhs.AsCStr() + startIndex, endIndex - startIndex);
}

//------------------------------------------------------------------------------
/**
 Construct string from raw data (a pointer, a start index and end index). 
 If endIndex is 0, ptr is expected to point to a null-terminated string, and 
 this string will be copied into the new string object.
*/
String::String(const char* ptr, int32 startIndex, int32 endIndex) {
    o_assert(nullptr != ptr);
    if (EndOfString == endIndex) {
        endIndex = std::strlen(ptr);
    }
    o_assert(startIndex < endIndex);
    this->create(ptr + startIndex, endIndex - startIndex);
}

//------------------------------------------------------------------------------
void
String::Assign(const char* ptr, int32 startIndex, int32 endIndex) {
    o_assert(nullptr != ptr);
    this->release();
    if (EndOfString == endIndex) {
        endIndex = std::strlen(ptr);
    }
    o_assert(startIndex < endIndex);
    this->create(ptr + startIndex, endIndex - startIndex);
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
    this->data = nullptr;
    this->strPtr = nullptr;
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
void
String::addRef() {
    o_assert(nullptr != this->data);
    #if ORYOL_HAS_ATOMIC
    this->data->refCount.fetch_add(1, std::memory_order_relaxed);
    #else
    this->data->refCount++;
    #endif
}

//------------------------------------------------------------------------------
void
String::release() {
    if (nullptr != this->data) {
        #if ORYOL_HAS_ATOMIC
        if (1 == this->data->refCount.fetch_sub(1, std::memory_order_relaxed)) {
        #else
        if (1 == this->data->refCount--) {
        #endif
            // no more owners, destroy the shared string data
            this->destroy();
        }
        this->data = nullptr;
        this->strPtr = nullptr;
    }
}

//------------------------------------------------------------------------------
/**
 Assign string from substring of other string. If len is 0, this
 means "until end of string".
 */
void
String::Assign(const String& rhs, int32 startIndex, int32 endIndex) {
    this->release();
    if (EndOfString == endIndex) {
        endIndex = rhs.Length();
    }
    o_assert((startIndex >= 0) && (startIndex < endIndex));
    o_assert(endIndex <= rhs.Length());
    this->create(rhs.AsCStr() + startIndex, endIndex - startIndex);
}
    
//------------------------------------------------------------------------------
String::String(const std::string& str) {
    this->create(str.c_str(), str.length());
}

//------------------------------------------------------------------------------
String::String(const String& rhs) {
    this->data = rhs.data;
    this->strPtr = rhs.strPtr;
    if (nullptr != this->data) {
        this->addRef();
    }
}

//------------------------------------------------------------------------------
String::String(String&& rhs) {
    this->data = rhs.data;
    this->strPtr = rhs.strPtr;
    rhs.data = 0;
    rhs.strPtr = 0;
}

//------------------------------------------------------------------------------
String::~String() {
    this->release();
}

//------------------------------------------------------------------------------
void
String::operator=(const char* str) {
    this->release();
    if (nullptr != str) {
        this->create(str, std::strlen(str));
    }
}

//------------------------------------------------------------------------------
void
String::operator=(const std::string& str) {
    this->release();
    this->create(str.c_str(), str.length());
}

//------------------------------------------------------------------------------
void
String::operator=(const String& rhs) {
    if (this != &rhs) {
        this->release();
        this->data = rhs.data;
        this->strPtr = rhs.strPtr;
        if (nullptr != this->data) {
            this->addRef();
        }
    }
}

//------------------------------------------------------------------------------
void
String::operator=(String&& rhs) {
    if (this != &rhs) {
        this->release();
        this->data = rhs.data;
        this->strPtr = rhs.strPtr;
        rhs.data = 0;
        rhs.strPtr = 0;
    }
}

//------------------------------------------------------------------------------
bool
String::operator==(const String& rhs) const {
    if (this->data == rhs.data) {
        return true;
    }
    else if ((nullptr == this->data) || (nullptr == rhs.data)) {
        return false;
    }
    else {
        return std::strcmp(this->AsCStr(), rhs.AsCStr()) == 0;
    }
}

//------------------------------------------------------------------------------
bool
String::operator!=(const String& rhs) const {
    return !this->operator==(rhs);
}

//------------------------------------------------------------------------------
bool
String::operator<(const String& rhs) const {
    if (this->data == rhs.data) {
        return false;
    }
    else {
        return std::strcmp(this->AsCStr(), rhs.AsCStr()) < 0;
    }
}

//------------------------------------------------------------------------------
bool
String::operator>(const String& rhs) const {
    if (this->data == rhs.data) {
        return false;
    }
    else {
        return std::strcmp(this->AsCStr(), rhs.AsCStr()) > 0;
    }
}

//------------------------------------------------------------------------------
bool
String::operator<=(const String& rhs) const {
    if (this->data == rhs.data) {
        return true;
    }
    else {
        return std::strcmp(this->AsCStr(), rhs.AsCStr()) <= 0;
    }
}

//------------------------------------------------------------------------------
bool
String::operator>=(const String& rhs) const {
    if (this->data == rhs.data) {
        return true;
    }
    else {
        return std::strcmp(this->AsCStr(), rhs.AsCStr()) >= 0;
    }
}

//------------------------------------------------------------------------------
int32
String::Length() const {
    if (nullptr == this->data) {
        return 0;
    }
    else {
        return this->data->length;
    }
}

//------------------------------------------------------------------------------
const char*
String::AsCStr() const {
    if (nullptr == this->strPtr) {
        return emptyString;
    }
    else {
        return this->strPtr;
    }
}

//------------------------------------------------------------------------------
std::string
String::AsStdString() const {
    return std::string(this->AsCStr());
}

//------------------------------------------------------------------------------
bool
String::IsValid() const {
    return this->AsCStr()[0] != 0;
}

//------------------------------------------------------------------------------
bool
String::Empty() const {
    return this->AsCStr()[0] == 0;
}

//------------------------------------------------------------------------------
void
String::Clear() {
    this->release();
}

//------------------------------------------------------------------------------
int32
String::RefCount() const {
    if (nullptr == this->data) {
        return 0;
    }
    else {
        return this->data->refCount;
    }
}

//------------------------------------------------------------------------------
char
String::Back() const {
    if (this->strPtr) {
        return this->strPtr[this->data->length - 1];
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
char
String::Front() const {
    if (this->strPtr) {
        return this->strPtr[0];
    }
    else {
        return 0;
    }
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

//------------------------------------------------------------------------------
bool operator==(const String& s0, const char* s1) {
    o_assert(0 != s1);
    return std::strcmp(s0.AsCStr(), s1) == 0;
}

//------------------------------------------------------------------------------
bool operator!=(const String& s0, const char* s1) {
    o_assert(0 != s1);
    return std::strcmp(s0.AsCStr(), s1) != 0;
}

//------------------------------------------------------------------------------
bool operator<(const String& s0, const char* s1) {
    o_assert(0 != s1);
    return std::strcmp(s0.AsCStr(), s1) < 0;
}

//------------------------------------------------------------------------------
bool operator>(const String& s0, const char* s1) {
    o_assert(0 != s1);
    return std::strcmp(s0.AsCStr(), s1) > 0;
}

//------------------------------------------------------------------------------
bool operator<=(const String& s0, const char* s1) {
    o_assert(0 != s1);
    return std::strcmp(s0.AsCStr(), s1) <= 0;
}

//------------------------------------------------------------------------------
bool operator>=(const String& s0, const char* s1) {
    o_assert(0 != s1);
    return std::strcmp(s0.AsCStr(), s1) >= 0;
}

//------------------------------------------------------------------------------
bool operator==(const char* s0, const String& s1) {
    o_assert(0 != s0);
    return std::strcmp(s0, s1.AsCStr()) == 0;
}

//------------------------------------------------------------------------------
bool operator!=(const char* s0, const String& s1) {
    o_assert(0 != s0);
    return std::strcmp(s0, s1.AsCStr()) != 0;
}

//------------------------------------------------------------------------------
bool operator<(const char* s0, const String& s1) {
    o_assert(0 != s0);
    return std::strcmp(s0, s1.AsCStr()) < 0;
}

//------------------------------------------------------------------------------
bool operator>(const char* s0, const String& s1) {
    o_assert(0 != s0);
    return std::strcmp(s0, s1.AsCStr()) > 0;
}

//------------------------------------------------------------------------------
bool operator<=(const char* s0, const String& s1) {
    o_assert(0 != s0);
    return std::strcmp(s0, s1.AsCStr()) <= 0;
}

//------------------------------------------------------------------------------
bool operator>=(const char* s0, const String& s1) {
    o_assert(0 != s0);
    return std::strcmp(s0, s1.AsCStr()) >= 0;
}

//------------------------------------------------------------------------------
bool operator==(const String& s0, const std::string& s1) {
    return std::strcmp(s0.AsCStr(), s1.c_str()) == 0;
}

//------------------------------------------------------------------------------
bool operator!=(const String& s0, const std::string& s1) {
    return std::strcmp(s0.AsCStr(), s1.c_str()) != 0;
}

//------------------------------------------------------------------------------
bool operator<(const String& s0, const std::string& s1) {
    return std::strcmp(s0.AsCStr(), s1.c_str()) < 0;
}

//------------------------------------------------------------------------------
bool operator>(const String& s0, const std::string& s1) {
    return std::strcmp(s0.AsCStr(), s1.c_str()) > 0;
}

//------------------------------------------------------------------------------
bool operator<=(const String& s0, const std::string& s1) {
    return std::strcmp(s0.AsCStr(), s1.c_str()) <= 0;
}

//------------------------------------------------------------------------------
bool operator>=(const String& s0, const std::string& s1) {
    return std::strcmp(s0.AsCStr(), s1.c_str()) >= 0;
}

//------------------------------------------------------------------------------
bool operator==(const std::string& s0, const String& s1) {
    return std::strcmp(s0.c_str(), s1.AsCStr()) == 0;
}

//------------------------------------------------------------------------------
bool operator!=(const std::string& s0, const String& s1) {
    return std::strcmp(s0.c_str(), s1.AsCStr()) == 0;
}

//------------------------------------------------------------------------------
bool operator<(const std::string& s0, const String& s1) {
    return std::strcmp(s0.c_str(), s1.AsCStr()) < 0;
}

//------------------------------------------------------------------------------
bool operator>(const std::string& s0, const String& s1) {
    return std::strcmp(s0.c_str(), s1.AsCStr()) > 0;
}

//------------------------------------------------------------------------------
bool operator<=(const std::string& s0, const String& s1) {
    return std::strcmp(s0.c_str(), s1.AsCStr()) <= 0;
}

//------------------------------------------------------------------------------
bool operator>=(const std::string& s0, const String& s1) {
    return std::strcmp(s0.c_str(), s1.AsCStr()) >= 0;
}

} // namespace Core
} // namespace Oryol