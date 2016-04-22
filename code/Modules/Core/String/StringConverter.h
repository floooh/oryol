#pragma once
//------------------------------------------------------------------------------
/*
    @class Oryol::String::StringConverter
    @brief various string conversion helpers

    StringConverter has methods to convert between UTF-8 and wide-strings,
    and from and to simple types (int, float, ...). Please note that
    wchar_t is 2 bytes (UTF-16) on Windows, but 4 bytes (UTF-32) 
    on other UNIX-like platforms!
*/
#include "Core/Types.h"
#include "Core/Containers/Array.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"
#include "Core/String/WideString.h"

namespace Oryol {
    
class StringConverter {
public:
    /// convert a string to simple type
    template<class TYPE> static TYPE FromString(const char* str);
    /// convert a string to simple type
    template<class TYPE> static TYPE FromString(const String& str);
    /// convert a string to simple type
    template<class TYPE> static TYPE FromString(const StringAtom& str);
    // (NOTE: 'ToString' methods should go into StringBuilder as Append<TYPE>()
    
    /// convert raw UTF8 string range to raw wide string
    static int UTF8ToWide(const unsigned char* src, int srcNumBytes, wchar_t* dst, int dstMaxBytes);
    /// convert raw wide string range to raw UTF8 string
    static int WideToUTF8(const wchar_t* src, int srcNumChars, unsigned char* dst, int dstMaxBytes);
    /// convert raw wide string range to UTF-8 string object
    static String WideToUTF8(const wchar_t* wide, int numWideChars);
    /// convert raw null-terminated wide-string to string object
    static String WideToUTF8(const wchar_t* wide);
    /// convert WideString object to UTF8 string object
    static String WideToUTF8(const WideString& wide);
    /// convert raw UTF8 string range to wide string object
    static WideString UTF8ToWide(const unsigned char* src, int srcNumBytes);
    /// convert null-terminated UTF8 raw string to wide string object
    static WideString UTF8ToWide(const unsigned char* src);
    /// convert UTF8 string object to wide string object
    static WideString UTF8ToWide(const String& src);

private:
    static const int MaxInternalBufferWChars = 128;
    static const int MaxUTF8Size = 6;
};

//------------------------------------------------------------------------------
template<class TYPE> inline TYPE
StringConverter::FromString(const String& str) {
    return FromString<TYPE>(str.AsCStr());
}

//------------------------------------------------------------------------------
template<class TYPE> inline TYPE
StringConverter::FromString(const StringAtom& str) {
    return FromString<TYPE>(str.AsCStr());
}

} // namespace Oryol
