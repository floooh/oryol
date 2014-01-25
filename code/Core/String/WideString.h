#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::WideString
    
    An immutable, shared wchar_t string class. Depending on platform,
    this means UTF-16 (in Windows) or UTF-32 (everywhere else).
    
    See String class for details.
    
    NOTE: usually one should work with UTF-8 string, and only convert 
    from and to WideString when needed. Use the StringUtil class to 
    convert between UTF encodings.
*/
#include "Core/Types.h"
#include "Core/Macros.h"

namespace Oryol {
namespace Core {
        
class StringAtom;
        
class WideString {
public:
    /// default constructor
    WideString();
    /// construct from wchar_t string (allocates!)
    WideString(const wchar_t* str);
    /// construct from raw wchar_t sequence
    WideString(const wchar_t* ptr, int32 numChars);
    /// construct from std::wstring (allocates!)
    WideString(const std::wstring& str);
    
    /// copy constructor (does not allocate)
    WideString(const WideString& rhs);
    /// move constructor (does not allocate)
    WideString(WideString&& rhs);
    
    /// assign from wchar_t string (allocates!)
    void operator=(const wchar_t* cstr);
    /// assign from std::wstring (allocates!)
    void operator=(const std::wstring& str);
    /// copy-assign from other WideString (does not allocate)
    void operator=(const WideString& rhs);
    /// move-assign from other WideString (does not allocate)
    void operator=(WideString&& rhs);
    
    /// equality operator
    bool operator==(const WideString& rhs) const;
    /// inequality operator
    bool operator!=(const WideString& rhs) const;
    /// less-than operator
    bool operator<(const WideString& rhs) const;
    /// greater-than operator
    bool operator>(const WideString& rhs) const;
    /// less-equal operator
    bool operator<=(const WideString& rhs) const;
    /// greater-equal operator
    bool operator>=(const WideString& rhs) const;
    
    /// assign from raw byte sequence, if numChars is 0 -> until first 0
    void Assign(const wchar_t* ptr, int32 numChars);
    /// get as C-String, will always return a valid ptr, even if String is empty
    const wchar_t* AsCStr() const;
    /// get as std::wstring (slow)
    std::wstring AsStdWString() const;
    
    /// get string length in number of wchar_t
    int32 Length() const;
    /// get byte length (Length() * sizeof(wchar_t)
    int32 ByteLength() const;
    /// return true if contains a non-empty string
    bool IsValid() const;
    /// return true if empty
    bool Empty() const;
    /// clear content
    void Clear();
    /// get the refcount of this string
    int32 RefCount() const;
    /// create an explicit copy of the string
    WideString MakeCopy() const;
    
private:
    /// shared string data header, this is followed by the actual string
    struct StringData {
        #if ORYOL_HAS_THREADS
        std::atomic<int32> refCount{0};
        #else
        int32 refCount{0};
        #endif
        int32 length;
    };
        
    /// create new string data block, len is number of characters (excluding 0 terminator
    void create(const wchar_t* ptr, int32 len);
    /// destroy shared string data block
    void destroy();
    /// increment refcount
    void addRef();
    /// decrement refcount, call destroy if 0
    void release();
    
    StringData* data;
    const wchar_t* strPtr;
    static const wchar_t* emptyString;
};

//------------------------------------------------------------------------------
inline bool operator==(const WideString& s0, const wchar_t* s1) {
    o_assert(0 != s1);
    return std::wcscmp(s0.AsCStr(), s1) == 0;
}
    
//------------------------------------------------------------------------------
inline bool operator!=(const WideString& s0, const wchar_t* s1) {
    o_assert(0 != s1);
    return std::wcscmp(s0.AsCStr(), s1) != 0;
}
    
//------------------------------------------------------------------------------
inline bool operator<(const WideString& s0, const wchar_t* s1) {
    o_assert(0 != s1);
    return std::wcscmp(s0.AsCStr(), s1) < 0;
}
    
//------------------------------------------------------------------------------
inline bool operator>(const WideString& s0, const wchar_t* s1) {
    o_assert(0 != s1);
    return std::wcscmp(s0.AsCStr(), s1) > 0;
}
    
//------------------------------------------------------------------------------
inline bool operator<=(const WideString& s0, const wchar_t* s1) {
    o_assert(0 != s1);
    return std::wcscmp(s0.AsCStr(), s1) <= 0;
}
    
//------------------------------------------------------------------------------
inline bool operator>=(const WideString& s0, const wchar_t* s1) {
    o_assert(0 != s1);
    return std::wcscmp(s0.AsCStr(), s1) >= 0;
}
    
//------------------------------------------------------------------------------
inline bool operator==(const wchar_t* s0, const WideString& s1) {
    o_assert(0 != s0);
    return std::wcscmp(s0, s1.AsCStr()) == 0;
}
    
//------------------------------------------------------------------------------
inline bool operator!=(const wchar_t* s0, const WideString& s1) {
    o_assert(0 != s0);
    return std::wcscmp(s0, s1.AsCStr()) != 0;
}
    
//------------------------------------------------------------------------------
inline bool operator<(const wchar_t* s0, const WideString& s1) {
    o_assert(0 != s0);
    return std::wcscmp(s0, s1.AsCStr()) < 0;
}
    
//------------------------------------------------------------------------------
inline bool operator>(const wchar_t* s0, const WideString& s1) {
    o_assert(0 != s0);
    return std::wcscmp(s0, s1.AsCStr()) > 0;
}
    
//------------------------------------------------------------------------------
inline bool operator<=(const wchar_t* s0, const WideString& s1) {
    o_assert(0 != s0);
    return std::wcscmp(s0, s1.AsCStr()) <= 0;
}
    
//------------------------------------------------------------------------------
inline bool operator>=(const wchar_t* s0, const WideString& s1) {
    o_assert(0 != s0);
    return std::wcscmp(s0, s1.AsCStr()) >= 0;
}
    
//------------------------------------------------------------------------------
inline bool operator==(const WideString& s0, const std::wstring& s1) {
    return std::wcscmp(s0.AsCStr(), s1.c_str()) == 0;
}
    
//------------------------------------------------------------------------------
inline bool operator!=(const WideString& s0, const std::wstring& s1) {
    return std::wcscmp(s0.AsCStr(), s1.c_str()) != 0;
}
    
//------------------------------------------------------------------------------
inline bool operator<(const WideString& s0, const std::wstring& s1) {
    return std::wcscmp(s0.AsCStr(), s1.c_str()) < 0;
}
    
//------------------------------------------------------------------------------
inline bool operator>(const WideString& s0, const std::wstring& s1) {
    return std::wcscmp(s0.AsCStr(), s1.c_str()) > 0;
}
    
//------------------------------------------------------------------------------
inline bool operator<=(const WideString& s0, const std::wstring& s1) {
    return std::wcscmp(s0.AsCStr(), s1.c_str()) <= 0;
}
    
//------------------------------------------------------------------------------
inline bool operator>=(const WideString& s0, const std::wstring& s1) {
    return std::wcscmp(s0.AsCStr(), s1.c_str()) >= 0;
}
    
//------------------------------------------------------------------------------
inline bool operator==(const std::wstring& s0, const WideString& s1) {
    return std::wcscmp(s0.c_str(), s1.AsCStr()) == 0;
}
    
//------------------------------------------------------------------------------
inline bool operator!=(const std::wstring& s0, const WideString& s1) {
    return std::wcscmp(s0.c_str(), s1.AsCStr()) == 0;
}

//------------------------------------------------------------------------------
inline bool operator<(const std::wstring& s0, const WideString& s1) {
    return std::wcscmp(s0.c_str(), s1.AsCStr()) < 0;
}
    
//------------------------------------------------------------------------------
inline bool operator>(const std::wstring& s0, const WideString& s1) {
    return std::wcscmp(s0.c_str(), s1.AsCStr()) > 0;
}
    
//------------------------------------------------------------------------------
inline bool operator<=(const std::wstring& s0, const WideString& s1) {
    return std::wcscmp(s0.c_str(), s1.AsCStr()) <= 0;
}

//------------------------------------------------------------------------------
inline bool operator>=(const std::wstring& s0, const WideString& s1) {
    return std::wcscmp(s0.c_str(), s1.AsCStr()) >= 0;
}

//------------------------------------------------------------------------------
inline void
WideString::addRef() {
    o_assert(nullptr != this->data);
    #if ORYOL_HAS_THREADS
    this->data->refCount.fetch_add(1, std::memory_order_relaxed);
    #else
    this->data->refCount++;
    #endif
}
    
//------------------------------------------------------------------------------
inline void
WideString::release() {
    if (nullptr != this->data) {
        #if ORYOL_HAS_THREADS
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
inline WideString::WideString() :
data(0),
strPtr(0) {
    // empty
}
    
//------------------------------------------------------------------------------
inline
WideString::WideString(const wchar_t* str) {
    o_assert(nullptr != str);
    this->create(str, std::wcslen(str));
}
    
//------------------------------------------------------------------------------
inline
WideString::WideString(const wchar_t* ptr, int32 numCharacters) {
    o_assert((nullptr != ptr) && (0 < numCharacters));
    this->create(ptr, numCharacters);
}
    
//------------------------------------------------------------------------------
inline
WideString::WideString(const std::wstring& str) {
    this->create(str.c_str(), str.length());
}
    
//------------------------------------------------------------------------------
inline
WideString::WideString(const WideString& rhs) {
    this->data = rhs.data;
    this->strPtr = rhs.strPtr;
    this->addRef();
}
    
//------------------------------------------------------------------------------
inline
WideString::WideString(WideString&& rhs) {
    this->data = rhs.data;
    this->strPtr = rhs.strPtr;
    rhs.data = 0;
    rhs.strPtr = 0;
}
    
//------------------------------------------------------------------------------
inline void
WideString::operator=(const wchar_t* str) {
    o_assert(0 != str);
    this->release();
    this->create(str, std::wcslen(str));
}
    
//------------------------------------------------------------------------------
inline void
WideString::operator=(const std::wstring& str) {
    this->release();
    this->create(str.c_str(), str.length());
}
    
//------------------------------------------------------------------------------
inline void
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
inline void
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
inline bool
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
inline bool
WideString::operator!=(const WideString& rhs) const {
    return !this->operator==(rhs);
}
    
//------------------------------------------------------------------------------
inline bool
WideString::operator<(const WideString& rhs) const {
    if (this->data == rhs.data) {
        return false;
    }
    else {
        return std::wcscmp(this->AsCStr(), rhs.AsCStr()) < 0;
    }
}
    
//------------------------------------------------------------------------------
inline bool
WideString::operator>(const WideString& rhs) const {
    if (this->data == rhs.data) {
        return false;
    }
    else {
        return std::wcscmp(this->AsCStr(), rhs.AsCStr()) > 0;
    }
}
    
//------------------------------------------------------------------------------
inline bool
WideString::operator<=(const WideString& rhs) const {
    if (this->data == rhs.data) {
        return true;
    }
    else {
        return std::wcscmp(this->AsCStr(), rhs.AsCStr()) <= 0;
    }
}
    
//------------------------------------------------------------------------------
inline bool
WideString::operator>=(const WideString& rhs) const {
    if (this->data == rhs.data) {
        return true;
    }
    else {
        return std::wcscmp(this->AsCStr(), rhs.AsCStr()) >= 0;
    }
}
    
//------------------------------------------------------------------------------
inline int32
WideString::Length() const {
    if (nullptr == this->data) {
        return 0;
    }
    else {
        return this->data->length;
    }
}

//------------------------------------------------------------------------------
inline int32
WideString::ByteLength() const {
    if (nullptr == this->data) {
        return 0;
    }
    else {
        return this->data->length * sizeof(wchar_t);
    }
}
    
//------------------------------------------------------------------------------
inline const wchar_t*
WideString::AsCStr() const {
    if (nullptr == this->strPtr) {
        return emptyString;
    }
    else {
        return this->strPtr;
    }
}
    
//------------------------------------------------------------------------------
inline std::wstring
WideString::AsStdWString() const {
    return std::wstring(this->AsCStr());
}
    
//------------------------------------------------------------------------------
inline bool
WideString::IsValid() const {
    return this->AsCStr()[0] != 0;
}
    
//------------------------------------------------------------------------------
inline bool
WideString::Empty() const {
    return this->AsCStr()[0] == 0;
}
    
//------------------------------------------------------------------------------
inline void
WideString::Clear() {
    this->release();
}

//------------------------------------------------------------------------------
inline int32
WideString::RefCount() const {
    if (nullptr == this->data) {
        return 0;
    }
    else {
        return this->data->refCount;
    }
}

//------------------------------------------------------------------------------
inline WideString
WideString::MakeCopy() const {
    return WideString(this->AsCStr());
}
    
//------------------------------------------------------------------------------
inline void
WideString::Assign(const wchar_t* ptr, int32 numChars) {
    o_assert(nullptr != ptr);
    this->release();
    if (0 == numChars) {
        numChars = std::wcslen(ptr);
    }
    this->create(ptr, numChars);
}

} // namespace Core
} // namespace Oryol
