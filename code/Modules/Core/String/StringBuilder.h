#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::StringBuilder
    @ingroup Core
    @brief manipulate and inspect string data
    
    Use the StringBuilder methods to build, manipulate and inspect
    string data. Internally a StringBuilder object has a dynamic
    buffer which grows as needed, but never shrinks.
*/
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Core/Containers/Array.h"

namespace Oryol {
    
class StringBuilder {
public:
    /// constructor
    StringBuilder();
    /// initialize from raw string
    StringBuilder(const char* str);
    /// initialize from raw character sequence, endIndex can be EndOfString
    StringBuilder(const char* str, int startIndex, int endIndex);
    /// initialize from string
    StringBuilder(const String& str);
    /// initialize from range of string, endIndex can be EndOfString
    StringBuilder(const String& str, int startIndex, int endIndex);
    /// initialize from an initializer list of strings
    StringBuilder(std::initializer_list<String> list);
    /// initialize from a list of strings with delimiters
    StringBuilder(char delim, std::initializer_list<String> list);
    /// destructor
    ~StringBuilder();
    
    /// reserve space (numBytes excludes the terminating 0 byte)
    void Reserve(int numBytes);
    /// get capacity
    int Capacity() const;
    /// get length (in bytes)
    int Length() const;
    /// clear the string builder
    void Clear();
    /// get the resulting string
    String GetString() const;
    /// get a substring, if endIndex can be EndOfString
    String GetSubString(int startIndex, int endIndex) const;
    /// get content as raw C string
    const char* AsCStr() const;
    
    /// printf-style formatting, max string length must be provided, returns false if resulting string is too long
    bool Format(int maxLength, const char* fmt, ...) __attribute__((format(printf, 3, 4)));
    /// printf-style formatting with existing va_list
    bool FormatVAList(int maxLength, const char* fmt, va_list args);
    /// same as Format, but append to existing content
    bool AppendFormat(int maxLength, const char* fmt, ...) __attribute__((format(printf, 3, 4)));
    /// same as Format with va_list, but append to existing content
    bool AppendFormatVAList(int maxLength, const char* fmt, va_list args);
    
    /// (re)set to raw null-terminated string
    void Set(const char* str);
    /// (re)set to raw character sequence, endIndex can be EndOfString
    void Set(const char* str, int startIndex, int endIndex);
    /// (re)set to string
    void Set(const String& str);
    /// (re)set to range from string, endIndex can be EndOfString
    void Set(const String& str, int startIndex, int endIndex);
    /// (re)set to a list of strings
    void Set(std::initializer_list<String> list);
    /// (re)set to a list of strings with delimiter
    void Set(char delim, std::initializer_list<String> list);
    
    /// append a single character
    void Append(char c);
    /// append raw null-terminated string
    void Append(const char* str);
    /// append raw character sequence, endIndex can be EndOfString
    void Append(const char* str, int startIndex, int endIndex);
    /// append string
    void Append(const String& str);
    /// append range from string, if endIndex can be EndOfString
    void Append(const String& str, int startIndex, int endIndex);
    /// append a list of strings
    void Append(std::initializer_list<String> list);
    /// append a list of strings with delimiter
    void Append(char delim, std::initializer_list<String> list);
    
    /// substitute all occurrences of a string, return number of substitutions
    int SubstituteAll(const char* match, const char* subst);
    /// substitute all, with String objects
    int SubstituteAll(const String& match, const String& subst);
    /// substitute first occurrence of a string, return true if substituted
    bool SubstituteFirst(const char* match, const char* subst);
    /// substitute first, with String objects
    bool SubstituteFirst(const String& match, const String& subst);
    /// substitute a range of characters with a string, endIndex can be EndOfString
    void SubstituteRange(int startIndex, int endIndex, const char* subst);
    
    /// find byte-index of first occurrence of delim chars, return InvalidIndex if not found
    int FindFirstOf(int startIndex, int endIndex, const char* delims) const;
    /// find byte-index of first occurrence of delim chars, return InvalidIndex if not found
    static int FindFirstOf(const char* str, int startIndex, int endIndex, const char* delims);
    /// find byte-index of first occurrence not in delim chars, return InvalidIndex if not found
    int FindFirstNotOf(int startIndex, int endIndex, const char* delims) const;
    /// find byte-index of first occurrence not in delim chars, return InvalidIndex if not found
    static int FindFirstNotOf(const char* str, int startIndex, int endIndex, const char* delims);

    /// find byte-index of last occurrence of delim chars, return InvalidIndex if not found
    int FindLastOf(int startIndex, int endIndex, const char* delims) const;
    /// find byte-index of last occurrence of delim chars, return InvalidIndex if not found
    static int FindLastOf(const char* str, int startIndex, int endIndex, const char* delims);
    /// find byte-index of last occurrence not in delim chars, return InvalidIndex if not found
    int FindLastNotOf(int startIndex, int endIndex, const char* delims) const;
    /// find byte-index of last occurrence not in delim chars, return InvalidIndex if not found
    static int FindLastNotOf(const char* str, int startIndex, int endIndex, const char* delims);

    /// find substring index, endIndex can be EndOfString, return InvalidIndex if not found
    int FindSubString(int startIndex, int endIndex, const char* subString) const;
    /// find substring index, endIndex can be EndOfString, return InvalidIndex if not found
    static int FindSubString(const char* str, int startIndex, int endIndex, const char* subString);
    /// test if contains a substring
    bool Contains(const char* str) const;
    /// test if string contains substring
    static bool Contains(const char* str, const char* subStr);

    /// tokenize content with a set of single characters as delimiters, this will clear the string builder content
    int Tokenize(const char* delims, Array<String>& outTokens);
    /// tokenize content, keep string within fence intact, this will clear the builder content
    int Tokenize(const char* delims, char fence, Array<String>& outTokens);
    
    /// truncate at index
    void Truncate(int index);
    /// get last character in content
    char Back() const;
    /// get character at index
    char At(int index) const;
    /// remove the last char
    char PopBack();

private:
    /// make sure that at least numBytes are available at end of string buffer 
    void ensureRoom(int numBytes);
    /// helper function for Substitute methods
    void substituteCommon(char* occur, int matchLen, int substLen, const char* subst);
    /// helper function for FindFirstOf functions
    static int findFirstOf(const char* str, int strLen, int startIndex, int endIndex, const char* delims);
    /// helper function for FindFirstNotOf functions
    static int findFirstNotOf(const char* str, int strLen, int startIndex, int endIndex, const char* delims);
    /// helper function for FindLastOf functions
    static int findLastOf(const char* str, int strLen, int startIndex, int endIndex, const char* delims);
    /// helper function for FindLastNotOf functions
    static int findLastNotOf(const char* str, int strLen, int startIndex, int endIndex, const char* delims);
    /// helper function for FindSubString functions
    static int findSubString(const char* str, int startIndex, int endIndex, const char* subStr);
    /// internal formatting method
    bool format(int maxLength, bool append, const char* fmt, va_list args);
    
    static const int minGrowSize = 128;
    char* buffer;
    int capacity;
    int size;
};
    
} // namespace Oryol
