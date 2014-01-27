#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::StringBuilder
    
    Helper class for dynamically building strings.
*/
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Core/Containers/Array.h"

namespace Oryol {
namespace Core {
    
class StringBuilder {
public:
    /// constructor
    StringBuilder();
    /// destructor
    ~StringBuilder();
    
    /// reserve space (numBytes excludes the terminating 0 byte)
    void Reserve(int32 numBytes);
    /// get capacity
    int32 Capacity() const;
    /// get length (in bytes)
    int32 Length() const;
    /// clear the string builder
    void Clear();
    /// get the resulting string
    String GetString() const;
    /// get a substring, if endIndex can be EndOfString
    String GetSubString(int32 startIndex, int32 endIndex) const;
    
    /// (re)set to raw null-terminated string
    void Set(const char* str);
    /// (re)set to raw character sequence, endIndex can be EndOfString
    void Set(const char* str, int32 startIndex, int32 endIndex);
    /// (re)set to string
    void Set(const String& str);
    /// (re)set to range from string, if endIndex can be EndOfString
    void Set(const String& str, int32 startIndex, int32 endIndex);
    /// (re)set to a list of strings
    void Set(std::initializer_list<String> list);
    /// (re)set to a list of strings with delimiter
    void Set(char delim, std::initializer_list<String> list);
    
    /// append a single character
    void Append(char c);
    /// append raw null-terminated string
    void Append(const char* str);
    /// append raw character sequence, endIndex can be EndOfString
    void Append(const char* str, int32 startIndex, int32 endIndex);
    /// append string
    void Append(const String& str);
    /// append range from string, if endIndex can be EndOfString
    void Append(const String& str, int32 startIndex, int32 endIndex);
    /// append a list of strings
    void Append(std::initializer_list<String> list);
    /// append a list of strings with delimiter
    void Append(char delim, std::initializer_list<String> list);
    
    /// substitute first occurance of a string with another, return true if substituted
    bool SubstituteFirst(const char* match, const char* subst);
    /// substitute, with String objects
    bool SubstituteFirst(const String& match, const String& subst);
    /// substitute a range of characters with a string, endIndex can be EndOfString
    void SubstituteRange(int32 startIndex, int32 endIndex, const char* subst);
    
    /// find byte-index of first occurence of delim chars, return EndOfString if not found
    int32 FindFirstOf(int32 startIndex, int32 endIndex, const char* delims) const;
    /// find byte-index of first occurence of delim chars, return EndOfString if not found
    static int32 FindFirstOf(const char* str, int32 startIndex, int32 endIndex, const char* delims);
    /// find byte-index of first occurence not in delim chars, return EndOfString if not found
    int32 FindFirstNotOf(int32 startIndex, int32 endIndex, const char* delims) const;
    /// find byte-index of first occurence not in delim chars, return EndOfString if not found
    static int32 FindFirstNotOf(const char* str, int32 startIndex, int32 endIndex, const char* delims);
    /// find substring index, endIndex can be EndOfString, return EndOfString if not found
    int32 FindSubString(int32 startIndex, int32 endIndex, const char* subString) const;
    /// find substring index, endIndex can be EndOfString, return EndOfString if not found
    static int32 FindSubString(const char* str, int32 startIndex, int32 endIndex, const char* subString);
    
    /// tokenize content with a set of single characters as delimiters, this will clear the string builder content
    int32 Tokenize(const char* delims, Array<String>& outTokens);
    /// tokenize content, keep string within fence intact, this will clear the builder content
    int32 Tokenize(const char* delims, char fence, Array<String>& outTokens);
    
    /// truncate at index
    void Truncate(int32 index);
    /// get last character in content
    char Back() const;
    /// get character at index
    char At(int32 index) const;
    /// remove the last char
    char PopBack();

    /// percent-encode content
    void PercentEncode();
    /// percent-decode content
    void PercentDecode();
    
private:
    /// make sure that at least numBytes are available at end of string buffer 
    void ensureRoom(int32 numBytes);
    /// helper function for Substitute methods
    void substituteCommon(char* occur, int32 matchLen, int32 substLen, const char* subst);
    /// helper function for FindFirstOf functions
    static int32 findFirstOf(const char* str, int32 strLen, int32 startIndex, int32 endIndex, const char* delims);
    /// helper function for FindFirstNotOf functions
    static int32 findFirstNotOf(const char* str, int32 strLen, int32 startIndex, int32 endIndex, const char* delims);
    /// helper function for FindSubString functions
    static int32 findSubString(const char* str, int32 startIndex, int32 endIndex, const char* subStr);
    
    static const int minGrowSize = 128;
    char* buffer;
    int32 capacity;
    int32 size;
};
    
} // namespace Core
} // namespace Oryol