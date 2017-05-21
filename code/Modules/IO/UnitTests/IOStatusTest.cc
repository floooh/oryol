//------------------------------------------------------------------------------
//  IOStatusTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/String/String.h"
#include "IO/IOTypes.h"

using namespace Oryol;

#define TOSTR(code) String(IOStatus::ToString(IOStatus::code)) == #code
#define FROMSTR(code) IOStatus::FromString(#code) == IOStatus::code

TEST(IOStatusTest) {

    CHECK(TOSTR(Continue));
    CHECK(TOSTR(SwitchingProtocols));
    CHECK(TOSTR(OK));
    CHECK(TOSTR(Created));
    CHECK(TOSTR(Accepted));
    CHECK(TOSTR(NonAuthorativeInformation));
    CHECK(TOSTR(NoContent));
    CHECK(TOSTR(ResetContent));
    CHECK(TOSTR(PartialContent));
    CHECK(TOSTR(MultipleChoices));
    CHECK(TOSTR(MovedPermanently));
    CHECK(TOSTR(Found));
    CHECK(TOSTR(SeeOther));
    CHECK(TOSTR(NotModified));
    CHECK(TOSTR(UseProxy));
    CHECK(TOSTR(TemporaryRedirect));
    CHECK(TOSTR(BadRequest));
    CHECK(TOSTR(Unauthorized));
    CHECK(TOSTR(PaymentRequired));
    CHECK(TOSTR(Forbidden));
    CHECK(TOSTR(NotFound));
    CHECK(TOSTR(MethodNotAllowed));
    CHECK(TOSTR(NotAcceptable));
    CHECK(TOSTR(ProxyAuthenticationRequired));
    CHECK(TOSTR(RequestTimeout));
    CHECK(TOSTR(Conflict));
    CHECK(TOSTR(Gone));
    CHECK(TOSTR(LengthRequired));
    CHECK(TOSTR(PreconditionFailed));
    CHECK(TOSTR(RequestEntityTooLarge));
    CHECK(TOSTR(RequestURITooLarge));
    CHECK(TOSTR(UnsupportedMediaType));
    CHECK(TOSTR(RequestedRangeNotSatisfiable));
    CHECK(TOSTR(ExpectationFailed));
    CHECK(TOSTR(InternalServerError));
    CHECK(TOSTR(NotImplemented));
    CHECK(TOSTR(BadGateway));
    CHECK(TOSTR(ServiceUnavailable));
    CHECK(TOSTR(GatewayTimeout));
    CHECK(TOSTR(HTTPVersionNotSupported));
    CHECK(TOSTR(Cancelled));
    CHECK(TOSTR(DownloadError));
}
