#pragma once
//------------------------------------------------------------------------------
/**
    @class String::StringUtil
    
    Various string utility functions.
*/
#include "Core/Types.h"

namespace Oryol {
namespace String {
    
class StringUtil {
public:
    /// tokenize a string with a set of single characters as delimiters
    static std::vector<std::string> Tokenize(const std::string& str, const char* delims);
    /// split string into two at first occurence of delimiter char 
    static std::pair<std::string,std::string> Bisect(const std::string& str, const char* delims);

    /// percent-encode a string
    static std::string PercentEncode(const std::string& src);
    /// percent-decode a string
    static std::string PercentDecode(const std::string& src);
};
    
} // namespace String
} // namespace Oryol