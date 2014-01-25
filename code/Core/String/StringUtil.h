#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::CoreStringUtil
    
    Various string utility functions.
*/
#include "Core/Types.h"
#include "Core/Containers/Array.h"
#include "Core/String/String.h"
#include "Core/String/WideString.h"

namespace Oryol {
namespace Core {
    
class StringUtil {
public:
    /// tokenize string with a set of single characters as delimiters
    static int32 Tokenize(const String& str, const char* delims, Array<String>& outTokens);
    /// tokenize string, keep string within fence intact
    static int32 Tokenize(const String& str, const char* delims, char fence, Array<String>& outTokens);
    /// split string into two at first occurence of delimiter char 
    static void Bisect(const String& str, const char* delims, String& outLeft, String& outRight);
    /// find byte-index of first occurence of delim chars, return InvalidIndex if not found
    static int32 FindFirstOf(const String& str, int32 startIndex, const char* delims);
    /// find byte-index of first occurence not in delim chars, return InvalidIndex if not found
    static int32 FindFirstNotOf(const String& str, int32 startIndex, const char* delims);
    /// concatenate strings
    static String Concatenate(char delim, std::initializer_list<String> list);
    /// append strings
    static String Append(std::initializer_list<String> list);
    /// truncate a string
    static String Truncate(const String& str, int32 newLen);

    /// percent-encode a string
    static String PercentEncode(const String& str);
    /// percent-decode a string
    static String PercentDecode(const String& str);
    /// convert a simple type to string
    template<class TYPE> static String ToString(const TYPE& val);
    /// convert a string to simple type
    template<class TYPE> static TYPE FromString(const String& str);
    /// convert an UTF-8 String to WideString
    static WideString UTF8ToWideString(const String& str);
    /// convert WideString to String
    static String WideToUTF8String(const WideString& str);
};
    
} // namespace Core
} // namespace Oryol