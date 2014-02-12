//------------------------------------------------------------------------------
//  IOStatus.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IOStatus.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace IO {
    
//------------------------------------------------------------------------------
const char*
IOStatus::ToString(Code c) {
    switch (c) {
        case Continue:              return "Continue";
        case OK:                    return "OK";
        case BadRequest:            return "BadRequest";
        case Forbidden:             return "Forbidden";
        case NotFound:              return "NotFound";
        case MethodNotAllowed:      return "MethodNotAllowed";
        case NotAcceptable:         return "NotAcceptable";
        case InternalServerError:   return "InternalServerError";
        case NotImplemented:        return "NotImplemented";
        case BadGateway:            return "BadGateway";
        case ServiceUnavailable:    return "ServiceUnavailable";
        
        default: return "InvalidIOStatus";
    }
}

//------------------------------------------------------------------------------
IOStatus::Code
IOStatus::FromString(const char* str) {
    o_assert(str);
    if (0 == std::strcmp(str, "Continue")) return Continue;
    else if (0 == std::strcmp(str, "OK")) return OK;
    else if (0 == std::strcmp(str, "BadRequest")) return BadRequest;
    else if (0 == std::strcmp(str, "Forbidden")) return Forbidden;
    else if (0 == std::strcmp(str, "NotFound")) return NotFound;
    else if (0 == std::strcmp(str, "MethodNotAllowed")) return MethodNotAllowed;
    else if (0 == std::strcmp(str, "NotAcceptable")) return NotAcceptable;
    else if (0 == std::strcmp(str, "InternalServerError")) return InternalServerError;
    else if (0 == std::strcmp(str, "NotImplemented")) return NotImplemented;
    else if (0 == std::strcmp(str, "BadGateway")) return BadGateway;
    else if (0 == std::strcmp(str, "ServiceUnavailable")) return ServiceUnavailable;
    else return InvalidIOStatus;
}
    
} // namespace IO
} // namespace Oryol