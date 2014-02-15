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
        OPTIONS = 0,
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        TRACE,
        CONNECT,
        
        NumHTTPMethods,
        InvalidHTTPMethod,
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace HTTP
} // namespace Oryol