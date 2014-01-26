#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::StringBuilder
    
    Helper class for dynamically building strings.
*/
#include "Core/Types.h"
#include "Core/String/String.h"

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
    /// get a substring, if endIndex is 0, until end-of-string
    String GetSubString(int32 startIndex, int32 endIndex) const;
    
    /// append a single character
    void Append(char c);
    /// append raw null-terminated string
    void Append(const char* str);
    /// append raw character sequence
    void Append(const char* str, int32 length);
    /// append string
    void Append(const String& str);
    /// append range from string, if endIndex is 0, copy until end of string
    void Append(const String& str, int32 startIndex, int32 endIndex);
    /// append a list of strings
    void Append(std::initializer_list<String> list);
    /// append a list of strings with delimiter
    void Append(char delim, std::initializer_list<String> list);
    
    /// truncate at index
    void Truncate(int32 index);
    /// remove the last char
    char PopBack();
    
private:
    /// make sure that at least numBytes are available at end of string buffer 
    void ensureRoom(int32 numBytes);
    
    static const int minGrowSize = 128;
    char* buffer;
    int32 capacity;
    int32 size;
};
    
} // namespace Core
} // namespace Oryol