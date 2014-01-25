//------------------------------------------------------------------------------
//  StringUtil.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Macros.h"
#include "StringUtil.h"

namespace Oryol {
namespace Core {

#if ORYOL_WINDOWS
#define o_strtok strtok_s
#else
#define o_strtok strtok_r
#endif
    
//------------------------------------------------------------------------------
std::vector<std::string>
StringUtil::Tokenize(const std::string& str, const char* delims) {
    o_assert(nullptr != delims);
    
    std::vector<std::string> tokens;
    std::string::size_type startIndex = 0;
    std::string::size_type endIndex = 0;
    const std::string::size_type len = str.length();
    if (len > 0) {
        std::string token;
        while (startIndex < len) {
        
            // skip white-space to next token
            startIndex = str.find_first_not_of(delims, startIndex);
            if (std::string::npos == startIndex) {
                // source string only consisted of delimiters
                startIndex = len;
            }
            else {
                endIndex = str.find_first_of(delims, startIndex);
                if (std::string::npos != endIndex) {
                    token.assign(str, startIndex, (endIndex - startIndex));
                    tokens.push_back(token);
                    startIndex = endIndex + 1;
                }
                else {
                    // at end of source string
                    token.assign(str, startIndex, std::string::npos);
                    tokens.push_back(token);
                    startIndex = endIndex;
                }
            }
        }
    }
    return tokens;
}

//------------------------------------------------------------------------------
std::pair<std::string, std::string>
StringUtil::Bisect(const std::string& src, const char* delims) {
    o_assert(nullptr != delims);
    
    std::string left;
    std::string right;
    auto splitIndex = src.find_first_of(delims);
    if (std::string::npos == splitIndex) {
        // no delimiters in string, return only left part
        left = src;
    }
    else {
        if (splitIndex > 0) {
            left.assign(src, 0, splitIndex);
        }
        // skip delimiters
        splitIndex = src.find_first_not_of(delims, splitIndex);
        if (std::string::npos != splitIndex) {
            right.assign(src, splitIndex, std::string::npos);
        }
    }
    return std::pair<std::string,std::string>(left, right);
}

} // namespace Core
} // namespace Oryol