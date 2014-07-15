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
        // 1xx Informational
        Continue = 100,
        SwitchingProtocols = 101,
        
        // 2xx Success
        OK = 200,
        Created = 201,
        Accepted = 202,
        NonAuthorativeInformation = 203,
        NoContent = 204,
        ResetContent = 205,
        PartialContent = 206,
        
        // 3xx Redirection
        MultipleChoices = 300,
        MovedPermanently = 301,
        Found = 302,
        SeeOther = 303,
        NotModified = 304,
        UseProxy = 305,
        TemporaryRedirect = 306,
        
        // 4xx Client Error
        BadRequest = 400,
        Unauthorized = 401,
        PaymentRequired = 402,
        Forbidden = 403,
        NotFound = 404,
        MethodNotAllowed = 405,
        NotAcceptable = 406,
        ProxyAuthenticationRequired = 407,
        RequestTimeout = 408,
        Conflict = 409,
        Gone = 410,
        LengthRequired = 411,
        PreconditionFailed = 412,
        RequestEntityTooLarge = 413,
        RequestURITooLarge = 414,
        UnsupportedMediaType = 415,
        RequestedRangeNotSatisfiable = 416,
        ExpectationFailed = 417,
        
        // 5xx Server Error
        InternalServerError = 500,
        NotImplemented = 501,
        BadGateway = 502,
        ServiceUnavailable = 503,
        GatewayTimeout = 504,
        HTTPVersionNotSupported = 505,
        
        // these are custom Oryol status codes
        Cancelled = 1000,
        DownloadError = 1001,
        
        InvalidIOStatus = InvalidIndex
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
  
} // namespace IO
} // namepsace Oryol