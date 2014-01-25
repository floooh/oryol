#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::StringBuilder
    
    Helper class for dynamically building strings.
*/
#include "Core/Types.h"
#include "Core/String.h"

namespace Oryol {
namespace Core {
    
class StringBuilder {
public:
    /// constructor
    StringBuilder();
    /// destructor
    ~StringBuilder();
    
    /// reserve space
    void Reserve(int32 numBytes);
    /// get capacity
    int32 Capacity() const;
    /// get length (in bytes)
    int32 Length() const;
    
    /// append a single character
    void Append(char c);
    /// append raw null-terminated string
    void Append(const char* str);
    /// append raw character sequence
    void Append(const char* str, int32 length);
    /// append string
    void Append(const String& str);
    /// append substring
    void Append(const String& str, int32 index, int32 length);
    /// append a list of strings
    void Append(std::initializer_list<String> list);
    /// append a list of strings with delimiter
    void Append(char delim, std::initializer_list<String> list);
    
    /// truncate at index
    void Truncate(int32 index);
    /// remove the last char
    void PopBack();
};
    
} // namespace Core
} // namespace Oryol