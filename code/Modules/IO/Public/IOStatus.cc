//------------------------------------------------------------------------------
//  IOStatus.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IOStatus.h"
#include "Core/Assertion.h"

namespace Oryol {

#define __ORYOL_TOSTRING(c) case c: return #c

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

} // namespace Oryol
