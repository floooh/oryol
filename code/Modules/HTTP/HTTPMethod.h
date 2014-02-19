#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::HTTP::HTTPMethod
    @brief HTTP methods enum class (GET, PUT, POST, ...)
    
    See http://tools.ietf.org/html/rfc2616
*/
#include "Core/Types.h"

namespace Oryol {
namespace HTTP {
    
class HTTPMethod {
public:
    /// methods enum
    enum Code {
        Options = 0,
        Get,
        Head,
        Post,
        Put,
        Delete,
        Trace,
        Connect,
        
        NumHTTPMethods,
        InvalidHTTPMethod,
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert to wide-string
    static const wchar_t* ToWideString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace HTTP
} // namespace Oryol