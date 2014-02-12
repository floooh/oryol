#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::IOStatus
    @brief IO request status codes
    
    IO request status codes, these are simply HTTP status codes.
*/
#include "Core/Types.h"

namespace Oryol {
namespace IO {

class IOStatus {
public:
    /// status code enum
    enum Code {
        Continue = 100,
        OK = 200,
        BadRequest = 400,
        Forbidden = 403,
        NotFound = 404,
        MethodNotAllowed = 405,
        NotAcceptable = 406,
        InternalServerError = 500,
        NotImplemented = 501,
        BadGateway = 502,
        ServiceUnavailable = 503,
        
        InvalidIOStatus = InvalidIndex
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
  
} // namespace IO
} // namepsace Oryol