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
#include <atomic>
#include <string>
#include "Core/Types.h"
#include "Core/Assert.h"

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
    
    /// destructor
    ~String();
    
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
    
private:
    /// shared string data header, this is followed by the actual string
    struct StringData {
        #if ORYOL_HAS_ATOMIC
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
bool operator==(const String& s0, const char* s1);
bool operator!=(const String& s0, const char* s1);
bool operator<(const String& s0, const char* s1);
bool operator>(const String& s0, const char* s1);
bool operator<=(const String& s0, const char* s1);
bool operator>=(const String& s0, const char* s1);
bool operator==(const char* s0, const String& s1);
bool operator!=(const char* s0, const String& s1);
bool operator<(const char* s0, const String& s1);
bool operator>(const char* s0, const String& s1);
bool operator<=(const char* s0, const String& s1);
bool operator>=(const char* s0, const String& s1);
bool operator==(const String& s0, const std::string& s1);
bool operator!=(const String& s0, const std::string& s1);
bool operator<(const String& s0, const std::string& s1);
bool operator>(const String& s0, const std::string& s1);
bool operator<=(const String& s0, const std::string& s1);
bool operator>=(const String& s0, const std::string& s1);
bool operator==(const std::string& s0, const String& s1);
bool operator!=(const std::string& s0, const String& s1);
bool operator<(const std::string& s0, const String& s1);
bool operator>(const std::string& s0, const String& s1);
bool operator<=(const std::string& s0, const String& s1);
bool operator>=(const std::string& s0, const String& s1);
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

} // namespace Core
} // namespace Oryol

