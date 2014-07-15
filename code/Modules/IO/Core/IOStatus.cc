//------------------------------------------------------------------------------
//  IOStatus.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IOStatus.h"
#include "Core/Macros.h"
#include "Core/Assert.h"
#include <cstring>

namespace Oryol {
namespace IO {
    
//------------------------------------------------------------------------------
const char*
IOStatus::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(Continue);
        __ORYOL_TOSTRING(SwitchingProtocols);
        __ORYOL_TOSTRING(OK);
        __ORYOL_TOSTRING(Created);
        __ORYOL_TOSTRING(Accepted);
        __ORYOL_TOSTRING(NonAuthorativeInformation);
        __ORYOL_TOSTRING(NoContent);
        __ORYOL_TOSTRING(ResetContent);
        __ORYOL_TOSTRING(PartialContent);
        __ORYOL_TOSTRING(MultipleChoices);
        __ORYOL_TOSTRING(MovedPermanently);
        __ORYOL_TOSTRING(Found);
        __ORYOL_TOSTRING(SeeOther);
        __ORYOL_TOSTRING(NotModified);
        __ORYOL_TOSTRING(UseProxy);
        __ORYOL_TOSTRING(TemporaryRedirect);
        __ORYOL_TOSTRING(BadRequest);
        __ORYOL_TOSTRING(Unauthorized);
        __ORYOL_TOSTRING(PaymentRequired);
        __ORYOL_TOSTRING(Forbidden);
        __ORYOL_TOSTRING(NotFound);
        __ORYOL_TOSTRING(MethodNotAllowed);
        __ORYOL_TOSTRING(NotAcceptable);
        __ORYOL_TOSTRING(ProxyAuthenticationRequired);
        __ORYOL_TOSTRING(RequestTimeout);
        __ORYOL_TOSTRING(Conflict);
        __ORYOL_TOSTRING(Gone);
        __ORYOL_TOSTRING(LengthRequired);
        __ORYOL_TOSTRING(PreconditionFailed);
        __ORYOL_TOSTRING(RequestEntityTooLarge);
        __ORYOL_TOSTRING(RequestURITooLarge);
        __ORYOL_TOSTRING(UnsupportedMediaType);
        __ORYOL_TOSTRING(RequestedRangeNotSatisfiable);
        __ORYOL_TOSTRING(ExpectationFailed);
        __ORYOL_TOSTRING(InternalServerError);
        __ORYOL_TOSTRING(NotImplemented);
        __ORYOL_TOSTRING(BadGateway);
        __ORYOL_TOSTRING(ServiceUnavailable);
        __ORYOL_TOSTRING(GatewayTimeout);
        __ORYOL_TOSTRING(HTTPVersionNotSupported);
        __ORYOL_TOSTRING(Cancelled);
        __ORYOL_TOSTRING(DownloadError);
        default: return "InvalidIOStatus";
    }
}

//------------------------------------------------------------------------------
IOStatus::Code
IOStatus::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(Continue);
    __ORYOL_FROMSTRING(SwitchingProtocols);
    __ORYOL_FROMSTRING(OK);
    __ORYOL_FROMSTRING(Created);
    __ORYOL_FROMSTRING(Accepted);
    __ORYOL_FROMSTRING(NonAuthorativeInformation);
    __ORYOL_FROMSTRING(NoContent);
    __ORYOL_FROMSTRING(ResetContent);
    __ORYOL_FROMSTRING(PartialContent);
    __ORYOL_FROMSTRING(MultipleChoices);
    __ORYOL_FROMSTRING(MovedPermanently);
    __ORYOL_FROMSTRING(Found);
    __ORYOL_FROMSTRING(SeeOther);
    __ORYOL_FROMSTRING(NotModified);
    __ORYOL_FROMSTRING(UseProxy);
    __ORYOL_FROMSTRING(TemporaryRedirect);
    __ORYOL_FROMSTRING(BadRequest);
    __ORYOL_FROMSTRING(Unauthorized);
    __ORYOL_FROMSTRING(PaymentRequired);
    __ORYOL_FROMSTRING(Forbidden);
    __ORYOL_FROMSTRING(NotFound);
    __ORYOL_FROMSTRING(MethodNotAllowed);
    __ORYOL_FROMSTRING(NotAcceptable);
    __ORYOL_FROMSTRING(ProxyAuthenticationRequired);
    __ORYOL_FROMSTRING(RequestTimeout);
    __ORYOL_FROMSTRING(Conflict);
    __ORYOL_FROMSTRING(Gone);
    __ORYOL_FROMSTRING(LengthRequired);
    __ORYOL_FROMSTRING(PreconditionFailed);
    __ORYOL_FROMSTRING(RequestEntityTooLarge);
    __ORYOL_FROMSTRING(RequestURITooLarge);
    __ORYOL_FROMSTRING(UnsupportedMediaType);
    __ORYOL_FROMSTRING(RequestedRangeNotSatisfiable);
    __ORYOL_FROMSTRING(ExpectationFailed);
    __ORYOL_FROMSTRING(InternalServerError);
    __ORYOL_FROMSTRING(NotImplemented);
    __ORYOL_FROMSTRING(BadGateway);
    __ORYOL_FROMSTRING(ServiceUnavailable);
    __ORYOL_FROMSTRING(GatewayTimeout);
    __ORYOL_FROMSTRING(HTTPVersionNotSupported);
    __ORYOL_FROMSTRING(Cancelled);
    __ORYOL_FROMSTRING(DownloadError);
    return InvalidIOStatus;
}
    
} // namespace IO
} // namespace Oryol