#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::String
    @brief immutable, reference counted, shared strings
    
    An immutable, shared UTF-8 String class. Memory is only allocated
    when creating or assigning from non-String objects (const char*,
    StringAtoms or std::string). When assigning from another string,
    only a pointer to the original string data is copied, and a 
    refcount is maintained. The last String pointing to the string
    data frees the string data.
    
    To manipulate string data, use the StringUtil class.
    
    NOTE: there is a constructor from a raw byte sequence, taking
    a pointer and a length. The pointed-to data may contain 0 bytes within
    the sequence, the String object will still allocate enough storage
    to contain the complete sequence (including 0-bytes), and String::Length()
    will return the number of bytes including those intermittent 0-bytes
    (but String::NumChars will return the number of UTF-8 character up to
    the first 0-byte). This is intended behaviour, don't change this!
    
    @see StringBuilder, StringAtom
*/
#include "Core/Types.h"
#include "Core/Macros.h"

namespace Oryol {
namespace Core {

class StringAtom;

class String {
public:
    /// default constructor
    String();
    /// construct from C string (allocates!)
    String(const char* cstr);
    /// construct from raw byte sequence, endIndex can be EndOfString
    String(const char* ptr, int32 startIndex, int32 endIndex);
    /// construct from substring of other string, endIndex can be EndOfString
    String(const String& rhs, int32 startIndex, int32 endIndex);
    /// construct from std::string (allocates!)
    String(const std::string& str);
    /// construct from StringAtom (allocates!)
    String(const StringAtom& str);
    
    /// copy constructor (does not allocate)
    String(const String& rhs);
    /// move constructor (does not allocate)
    String(String&& rhs);
    
    /// assign from C string (allocates!)
    void operator=(const char* cstr);
    /// assign from std::string (allocates!)
    void operator=(const std::string& str);
    /// assign from StringAtom (allocates!)
    void operator=(const StringAtom& str);
    /// copy-assign from other String (does not allocate)
    void operator=(const String& rhs);
    /// move-assign from other String (does not allocate)
    void operator=(String&& rhs);
    
    /// equality operator
    bool operator==(const String& rhs) const;
    /// inequality operator
    bool operator!=(const String& rhs) const;
    /// less-than operator
    bool operator<(const String& rhs) const;
    /// greater-than operator
    bool operator>(const String& rhs) const;
    /// less-equal operator
    bool operator<=(const String& rhs) const;
    /// greater-equal operator
    bool operator>=(const String& rhs) const;

    /// allocate room for numCharBytes, and return pointer
    char* Alloc(int32 numCharBytes);
    /// assign from raw byte sequence, endIndex can be EndOfString
    void Assign(const char* ptr, int32 startIndex, int32 endIndex);
    /// assign from other string, with start index and endIndex, endIndex can be EndOfString
    void Assign(const String& rhs, int32 startIndex, int32 endIndex);
    /// get as C-String, will always return a valid ptr, even if String is empty
    const char* AsCStr() const;
    /// get as std::string (slow)
    std::string AsStdString() const;
    /// get as StringAtom (slow)
    StringAtom AsStringAtom() const;
    /// get the last character in the string
    char Back() const;
    /// get the first character in the string
    char Front() const;
    
    /// get string length in number of bytes
    int32 Length() const;
    /// return true if contains a non-empty string
    bool IsValid() const;
    /// return true if empty
    bool Empty() const;
    /// clear content
    void Clear();
    /// get the refcount of this string
    int32 RefCount() const;
    /// create an explicit copy of the string
    String MakeCopy() const;
    
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
    
    /// create new string data block, numBytes does not include the terminating 0
    void create(const char* ptr, int32 len);
    /// private alloc function for len
    void alloc(int32 len);
    /// destroy shared string data block
    void destroy();
    /// increment refcount
    void addRef();
    /// decrement refcount, call destroy if 0
    void release();
    
    StringData* data;
    const char* strPtr;     // direct pointer to string data, necessary to see something in the debugger
    static const char* emptyString;
};

//------------------------------------------------------------------------------
inline bool operator==(const String& s0, const char* s1) {
    o_assert(0 != s1);
    return std::strcmp(s0.AsCStr(), s1) == 0;
}

//------------------------------------------------------------------------------
inline bool operator!=(const String& s0, const char* s1) {
    o_assert(0 != s1);
    return std::strcmp(s0.AsCStr(), s1) != 0;
}

//------------------------------------------------------------------------------
inline bool operator<(const String& s0, const char* s1) {
    o_assert(0 != s1);
    return std::strcmp(s0.AsCStr(), s1) < 0;
}

//------------------------------------------------------------------------------
inline bool operator>(const String& s0, const char* s1) {
    o_assert(0 != s1);
    return std::strcmp(s0.AsCStr(), s1) > 0;
}

//------------------------------------------------------------------------------
inline bool operator<=(const String& s0, const char* s1) {
    o_assert(0 != s1);
    return std::strcmp(s0.AsCStr(), s1) <= 0;
}

//------------------------------------------------------------------------------
inline bool operator>=(const String& s0, const char* s1) {
    o_assert(0 != s1);
    return std::strcmp(s0.AsCStr(), s1) >= 0;
}

//------------------------------------------------------------------------------
inline bool operator==(const char* s0, const String& s1) {
    o_assert(0 != s0);
    return std::strcmp(s0, s1.AsCStr()) == 0;
}

//------------------------------------------------------------------------------
inline bool operator!=(const char* s0, const String& s1) {
    o_assert(0 != s0);
    return std::strcmp(s0, s1.AsCStr()) != 0;
}

//------------------------------------------------------------------------------
inline bool operator<(const char* s0, const String& s1) {
    o_assert(0 != s0);
    return std::strcmp(s0, s1.AsCStr()) < 0;
}

//------------------------------------------------------------------------------
inline bool operator>(const char* s0, const String& s1) {
    o_assert(0 != s0);
    return std::strcmp(s0, s1.AsCStr()) > 0;
}

//------------------------------------------------------------------------------
inline bool operator<=(const char* s0, const String& s1) {
    o_assert(0 != s0);
    return std::strcmp(s0, s1.AsCStr()) <= 0;
}

//------------------------------------------------------------------------------
inline bool operator>=(const char* s0, const String& s1) {
    o_assert(0 != s0);
    return std::strcmp(s0, s1.AsCStr()) >= 0;
}

//------------------------------------------------------------------------------
inline bool operator==(const String& s0, const std::string& s1) {
    return std::strcmp(s0.AsCStr(), s1.c_str()) == 0;
}

//------------------------------------------------------------------------------
inline bool operator!=(const String& s0, const std::string& s1) {
    return std::strcmp(s0.AsCStr(), s1.c_str()) != 0;
}

//------------------------------------------------------------------------------
inline bool operator<(const String& s0, const std::string& s1) {
    return std::strcmp(s0.AsCStr(), s1.c_str()) < 0;
}

//------------------------------------------------------------------------------
inline bool operator>(const String& s0, const std::string& s1) {
    return std::strcmp(s0.AsCStr(), s1.c_str()) > 0;
}

//------------------------------------------------------------------------------
inline bool operator<=(const String& s0, const std::string& s1) {
    return std::strcmp(s0.AsCStr(), s1.c_str()) <= 0;
}

//------------------------------------------------------------------------------
inline bool operator>=(const String& s0, const std::string& s1) {
    return std::strcmp(s0.AsCStr(), s1.c_str()) >= 0;
}

//------------------------------------------------------------------------------
inline bool operator==(const std::string& s0, const String& s1) {
    return std::strcmp(s0.c_str(), s1.AsCStr()) == 0;
}

//------------------------------------------------------------------------------
inline bool operator!=(const std::string& s0, const String& s1) {
    return std::strcmp(s0.c_str(), s1.AsCStr()) == 0;
}

//------------------------------------------------------------------------------
inline bool operator<(const std::string& s0, const String& s1) {
    return std::strcmp(s0.c_str(), s1.AsCStr()) < 0;
}

//------------------------------------------------------------------------------
inline bool operator>(const std::string& s0, const String& s1) {
    return std::strcmp(s0.c_str(), s1.AsCStr()) > 0;
}

//------------------------------------------------------------------------------
inline bool operator<=(const std::string& s0, const String& s1) {
    return std::strcmp(s0.c_str(), s1.AsCStr()) <= 0;
}

//------------------------------------------------------------------------------
inline bool operator>=(const std::string& s0, const String& s1) {
    return std::strcmp(s0.c_str(), s1.AsCStr()) >= 0;
}

//------------------------------------------------------------------------------
bool operator==(const String& s0, const StringAtom& s1);
bool operator!=(const String& s0, const StringAtom& s1);
bool operator<(const String& s0, const StringAtom& s1);
bool operator>(const String& s0, const StringAtom& s1);
bool operator<=(const String& s0, const StringAtom& s1);
bool operator>=(const String& s0, const StringAtom& s1);
bool operator==(const StringAtom& s0, const String& s1);
bool operator!=(const StringAtom& s0, const String& s1);
bool operator<(const StringAtom& s0, const String& s1);
bool operator>(const StringAtom& s0, const String& s1);
bool operator<=(const StringAtom& s0, const String& s1);
bool operator>=(const StringAtom& s0, const String& s1);

//------------------------------------------------------------------------------
inline void
String::addRef() {
    o_assert(nullptr != this->data);
    #if ORYOL_HAS_THREADS
    this->data->refCount.fetch_add(1, std::memory_order_relaxed);
    #else
    this->data->refCount++;
    #endif
}

//------------------------------------------------------------------------------
inline void
String::release() {
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
        this->strPtr = 0;
    }
}

//------------------------------------------------------------------------------
inline String::String() :
data(nullptr),
strPtr(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
inline
String::String(const char* str) {
    o_assert(nullptr != str);
    this->create(str, std::strlen(str));
}

//------------------------------------------------------------------------------
/**
 Construct string from raw data (a pointer, a start index and end index). 
 If endIndex is 0, ptr is expected to point to a null-terminated string, and 
 this string will be copied into the new string object.
*/
inline
String::String(const char* ptr, int32 startIndex, int32 endIndex) {
    o_assert(nullptr != ptr);
    if (EndOfString == endIndex) {
        endIndex = std::strlen(ptr);
    }
    o_assert(startIndex < endIndex);
    this->create(ptr + startIndex, endIndex - startIndex);
}

//------------------------------------------------------------------------------
inline void
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
/**
 Construct string from substring of other string. If len is 0, this
 means "until end of string".
*/
inline
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
 Assign string from substring of other string. If len is 0, this
 means "until end of string".
 */
inline void
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
inline
String::String(const std::string& str) {
    this->create(str.c_str(), str.length());
}

//------------------------------------------------------------------------------
inline
String::String(const String& rhs) {
    this->data = rhs.data;
    this->strPtr = rhs.strPtr;
    this->addRef();
}

//------------------------------------------------------------------------------
inline
String::String(String&& rhs) {
    this->data = rhs.data;
    this->strPtr = rhs.strPtr;
    rhs.data = 0;
    rhs.strPtr = 0;
}

//------------------------------------------------------------------------------
inline void
String::operator=(const char* str) {
    o_assert(0 != str);
    this->release();
    this->create(str, std::strlen(str));
}

//------------------------------------------------------------------------------
inline void
String::operator=(const std::string& str) {
    this->release();
    this->create(str.c_str(), str.length());
}

//------------------------------------------------------------------------------
inline void
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
inline void
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
inline bool
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
inline bool
String::operator!=(const String& rhs) const {
    return !this->operator==(rhs);
}

//------------------------------------------------------------------------------
inline bool
String::operator<(const String& rhs) const {
    if (this->data == rhs.data) {
        return false;
    }
    else {
        return std::strcmp(this->AsCStr(), rhs.AsCStr()) < 0;
    }
}

//------------------------------------------------------------------------------
inline bool
String::operator>(const String& rhs) const {
    if (this->data == rhs.data) {
        return false;
    }
    else {
        return std::strcmp(this->AsCStr(), rhs.AsCStr()) > 0;
    }
}

//------------------------------------------------------------------------------
inline bool
String::operator<=(const String& rhs) const {
    if (this->data == rhs.data) {
        return true;
    }
    else {
        return std::strcmp(this->AsCStr(), rhs.AsCStr()) <= 0;
    }
}

//------------------------------------------------------------------------------
inline bool
String::operator>=(const String& rhs) const {
    if (this->data == rhs.data) {
        return true;
    }
    else {
        return std::strcmp(this->AsCStr(), rhs.AsCStr()) >= 0;
    }
}

//------------------------------------------------------------------------------
inline int32
String::Length() const {
    if (nullptr == this->data) {
        return 0;
    }
    else {
        return this->data->length;
    }
}

//------------------------------------------------------------------------------
inline const char*
String::AsCStr() const {
    if (nullptr == this->strPtr) {
        return emptyString;
    }
    else {
        return this->strPtr;
    }
}

//------------------------------------------------------------------------------
inline std::string
String::AsStdString() const {
    return std::string(this->AsCStr());
}

//------------------------------------------------------------------------------
inline bool
String::IsValid() const {
    return this->AsCStr()[0] != 0;
}

//------------------------------------------------------------------------------
inline bool
String::Empty() const {
    return this->AsCStr()[0] == 0;
}

//------------------------------------------------------------------------------
inline void
String::Clear() {
    this->release();
}

//------------------------------------------------------------------------------
inline int32
String::RefCount() const {
    if (nullptr == this->data) {
        return 0;
    }
    else {
        return this->data->refCount;
    }
}

//------------------------------------------------------------------------------
inline String
String::MakeCopy() const {
    return String(this->AsCStr());
}

//------------------------------------------------------------------------------
inline char
String::Back() const {
    if (this->strPtr) {
        return this->strPtr[this->data->length - 1];
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
inline char
String::Front() const {
    if (this->strPtr) {
        return this->strPtr[0];
    }
    else {
        return 0;
    }
}

} // namespace Core
} // namespace Oryol

