//------------------------------------------------------------------------------
//  WideString.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include <cwchar>
#include "WideString.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace Core {

const wchar_t* WideString::emptyString = L"";
    
//------------------------------------------------------------------------------
void
WideString::destroy() {
    o_assert(0 != this->data);
    o_assert(0 == this->data->refCount);
    this->data->~StringData();
    Memory::Free(this->data);
    this->data = 0;
}

//------------------------------------------------------------------------------
void
WideString::create(const wchar_t* ptr, int32 numChars) {
    o_assert(0 != ptr);
    if ((ptr[0] != 0) && (numChars > 0)) {
        this->data = (StringData*) Memory::Alloc(sizeof(StringData) + ((numChars + 1) * sizeof(wchar_t)));
        new(this->data) StringData();
        this->addRef();
        this->data->length = numChars;
        this->strPtr = (const wchar_t*) &(this->data[1]);
        Memory::Copy(ptr, (void*) this->strPtr, numChars * sizeof(wchar_t));
        ((wchar_t*)strPtr)[numChars] = 0;
    }
    else {
        // empty string, don't bother to allocate storage for this
        this->data = nullptr;
        this->strPtr = 0;
    }
}

//------------------------------------------------------------------------------
void
WideString::addRef() {
    o_assert(nullptr != this->data);
    #if ORYOL_HAS_ATOMIC
    this->data->refCount.fetch_add(1, std::memory_order_relaxed);
    #else
    this->data->refCount++;
    #endif
}
    
//------------------------------------------------------------------------------
void
WideString::release() {
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
WideString::WideString() :
data(0),
strPtr(0) {
    // empty
}
    
//------------------------------------------------------------------------------
WideString::WideString(const wchar_t* str) {
    o_assert(nullptr != str);
    this->create(str, std::wcslen(str));
}
    
//------------------------------------------------------------------------------
WideString::WideString(const wchar_t* ptr, int32 numCharacters) {
    o_assert((nullptr != ptr) && (0 < numCharacters));
    this->create(ptr, numCharacters);
}
    
//------------------------------------------------------------------------------
WideString::WideString(const std::wstring& str) {
    this->create(str.c_str(), str.length());
}
    
//------------------------------------------------------------------------------
WideString::WideString(const WideString& rhs) {
    this->data = rhs.data;
    this->strPtr = rhs.strPtr;
    this->addRef();
}
    
//------------------------------------------------------------------------------
WideString::WideString(WideString&& rhs) {
    this->data = rhs.data;
    this->strPtr = rhs.strPtr;
    rhs.data = 0;
    rhs.strPtr = 0;
}

//------------------------------------------------------------------------------
WideString::~WideString() {
    this->release();
}

//------------------------------------------------------------------------------
void
WideString::operator=(const wchar_t* str) {
    o_assert(0 != str);
    this->release();
    this->create(str, std::wcslen(str));
}
    
//------------------------------------------------------------------------------
void
WideString::operator=(const std::wstring& str) {
    this->release();
    this->create(str.c_str(), str.length());
}
    
//------------------------------------------------------------------------------
void
WideString::operator=(const WideString& rhs) {
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
WideString::operator=(WideString&& rhs) {
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
WideString::operator==(const WideString& rhs) const {
    if (this->data == rhs.data) {
        return true;
    }
    else if ((nullptr == this->data) || (nullptr == rhs.data)) {
        return false;
    }
    else {
        return std::wcscmp(this->AsCStr(), rhs.AsCStr()) == 0;
    }
}
    
//------------------------------------------------------------------------------
bool
WideString::operator!=(const WideString& rhs) const {
    return !this->operator==(rhs);
}
    
//------------------------------------------------------------------------------
bool
WideString::operator<(const WideString& rhs) const {
    if (this->data == rhs.data) {
        return false;
    }
    else {
        return std::wcscmp(this->AsCStr(), rhs.AsCStr()) < 0;
    }
}
    
//------------------------------------------------------------------------------
bool
WideString::operator>(const WideString& rhs) const {
    if (this->data == rhs.data) {
        return false;
    }
    else {
        return std::wcscmp(this->AsCStr(), rhs.AsCStr()) > 0;
    }
}
    
//------------------------------------------------------------------------------
bool
WideString::operator<=(const WideString& rhs) const {
    if (this->data == rhs.data) {
        return true;
    }
    else {
        return std::wcscmp(this->AsCStr(), rhs.AsCStr()) <= 0;
    }
}
    
//------------------------------------------------------------------------------
bool
WideString::operator>=(const WideString& rhs) const {
    if (this->data == rhs.data) {
        return true;
    }
    else {
        return std::wcscmp(this->AsCStr(), rhs.AsCStr()) >= 0;
    }
}
    
//------------------------------------------------------------------------------
int32
WideString::Length() const {
    if (nullptr == this->data) {
        return 0;
    }
    else {
        return this->data->length;
    }
}

//------------------------------------------------------------------------------
int32
WideString::ByteLength() const {
    if (nullptr == this->data) {
        return 0;
    }
    else {
        return this->data->length * sizeof(wchar_t);
    }
}
    
//------------------------------------------------------------------------------
const wchar_t*
WideString::AsCStr() const {
    if (nullptr == this->strPtr) {
        return emptyString;
    }
    else {
        return this->strPtr;
    }
}
    
//------------------------------------------------------------------------------
std::wstring
WideString::AsStdWString() const {
    return std::wstring(this->AsCStr());
}
    
//------------------------------------------------------------------------------
bool
WideString::IsValid() const {
    return this->AsCStr()[0] != 0;
}
    
//------------------------------------------------------------------------------
bool
WideString::Empty() const {
    return this->AsCStr()[0] == 0;
}
    
//------------------------------------------------------------------------------
void
WideString::Clear() {
    this->release();
}

//------------------------------------------------------------------------------
int32
WideString::RefCount() const {
    if (nullptr == this->data) {
        return 0;
    }
    else {
        return this->data->refCount;
    }
}

//------------------------------------------------------------------------------
void
WideString::Assign(const wchar_t* ptr, int32 numChars) {
    o_assert(nullptr != ptr);
    this->release();
    if (0 == numChars) {
        numChars = std::wcslen(ptr);
    }
    this->create(ptr, numChars);
}

//------------------------------------------------------------------------------
bool operator==(const WideString& s0, const wchar_t* s1) {
    o_assert(0 != s1);
    return std::wcscmp(s0.AsCStr(), s1) == 0;
}
    
//------------------------------------------------------------------------------
bool operator!=(const WideString& s0, const wchar_t* s1) {
    o_assert(0 != s1);
    return std::wcscmp(s0.AsCStr(), s1) != 0;
}
    
//------------------------------------------------------------------------------
bool operator<(const WideString& s0, const wchar_t* s1) {
    o_assert(0 != s1);
    return std::wcscmp(s0.AsCStr(), s1) < 0;
}
    
//------------------------------------------------------------------------------
bool operator>(const WideString& s0, const wchar_t* s1) {
    o_assert(0 != s1);
    return std::wcscmp(s0.AsCStr(), s1) > 0;
}
    
//------------------------------------------------------------------------------
bool operator<=(const WideString& s0, const wchar_t* s1) {
    o_assert(0 != s1);
    return std::wcscmp(s0.AsCStr(), s1) <= 0;
}
    
//------------------------------------------------------------------------------
bool operator>=(const WideString& s0, const wchar_t* s1) {
    o_assert(0 != s1);
    return std::wcscmp(s0.AsCStr(), s1) >= 0;
}
    
//------------------------------------------------------------------------------
bool operator==(const wchar_t* s0, const WideString& s1) {
    o_assert(0 != s0);
    return std::wcscmp(s0, s1.AsCStr()) == 0;
}
    
//------------------------------------------------------------------------------
bool operator!=(const wchar_t* s0, const WideString& s1) {
    o_assert(0 != s0);
    return std::wcscmp(s0, s1.AsCStr()) != 0;
}
    
//------------------------------------------------------------------------------
bool operator<(const wchar_t* s0, const WideString& s1) {
    o_assert(0 != s0);
    return std::wcscmp(s0, s1.AsCStr()) < 0;
}
    
//------------------------------------------------------------------------------
bool operator>(const wchar_t* s0, const WideString& s1) {
    o_assert(0 != s0);
    return std::wcscmp(s0, s1.AsCStr()) > 0;
}
    
//------------------------------------------------------------------------------
bool operator<=(const wchar_t* s0, const WideString& s1) {
    o_assert(0 != s0);
    return std::wcscmp(s0, s1.AsCStr()) <= 0;
}
    
//------------------------------------------------------------------------------
bool operator>=(const wchar_t* s0, const WideString& s1) {
    o_assert(0 != s0);
    return std::wcscmp(s0, s1.AsCStr()) >= 0;
}
    
//------------------------------------------------------------------------------
bool operator==(const WideString& s0, const std::wstring& s1) {
    return std::wcscmp(s0.AsCStr(), s1.c_str()) == 0;
}
    
//------------------------------------------------------------------------------
bool operator!=(const WideString& s0, const std::wstring& s1) {
    return std::wcscmp(s0.AsCStr(), s1.c_str()) != 0;
}
    
//------------------------------------------------------------------------------
bool operator<(const WideString& s0, const std::wstring& s1) {
    return std::wcscmp(s0.AsCStr(), s1.c_str()) < 0;
}
    
//------------------------------------------------------------------------------
bool operator>(const WideString& s0, const std::wstring& s1) {
    return std::wcscmp(s0.AsCStr(), s1.c_str()) > 0;
}
    
//------------------------------------------------------------------------------
bool operator<=(const WideString& s0, const std::wstring& s1) {
    return std::wcscmp(s0.AsCStr(), s1.c_str()) <= 0;
}
    
//------------------------------------------------------------------------------
bool operator>=(const WideString& s0, const std::wstring& s1) {
    return std::wcscmp(s0.AsCStr(), s1.c_str()) >= 0;
}
    
//------------------------------------------------------------------------------
bool operator==(const std::wstring& s0, const WideString& s1) {
    return std::wcscmp(s0.c_str(), s1.AsCStr()) == 0;
}
    
//------------------------------------------------------------------------------
bool operator!=(const std::wstring& s0, const WideString& s1) {
    return std::wcscmp(s0.c_str(), s1.AsCStr()) == 0;
}

//------------------------------------------------------------------------------
bool operator<(const std::wstring& s0, const WideString& s1) {
    return std::wcscmp(s0.c_str(), s1.AsCStr()) < 0;
}
    
//------------------------------------------------------------------------------
bool operator>(const std::wstring& s0, const WideString& s1) {
    return std::wcscmp(s0.c_str(), s1.AsCStr()) > 0;
}
    
//------------------------------------------------------------------------------
bool operator<=(const std::wstring& s0, const WideString& s1) {
    return std::wcscmp(s0.c_str(), s1.AsCStr()) <= 0;
}

//------------------------------------------------------------------------------
bool operator>=(const std::wstring& s0, const WideString& s1) {
    return std::wcscmp(s0.c_str(), s1.AsCStr()) >= 0;
}
    
} // namespace Core
} // namespace Oryol