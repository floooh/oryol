//------------------------------------------------------------------------------
//  IOStatusTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/String/String.h"
#include "IO/IOStatus.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::IO;

TEST(IOStatusTest) {

    CHECK(String(IOStatus::ToString(IOStatus::Continue)) == "Continue");
    CHECK(String(IOStatus::ToString(IOStatus::OK)) == "OK");
    CHECK(String(IOStatus::ToString(IOStatus::BadRequest)) == "BadRequest");
    CHECK(String(IOStatus::ToString(IOStatus::Forbidden)) == "Forbidden");
    CHECK(String(IOStatus::ToString(IOStatus::NotFound)) == "NotFound");
    CHECK(String(IOStatus::ToString(IOStatus::MethodNotAllowed)) == "MethodNotAllowed");
    CHECK(String(IOStatus::ToString(IOStatus::NotAcceptable)) == "NotAcceptable");
    CHECK(String(IOStatus::ToString(IOStatus::InternalServerError)) == "InternalServerError");
    CHECK(String(IOStatus::ToString(IOStatus::NotImplemented)) == "NotImplemented");
    CHECK(String(IOStatus::ToString(IOStatus::BadGateway)) == "BadGateway");
    CHECK(String(IOStatus::ToString(IOStatus::ServiceUnavailable)) == "ServiceUnavailable");
    CHECK(String(IOStatus::ToString((IOStatus::Code)1234)) == "InvalidIOStatus");
    
    
    CHECK(IOStatus::FromString("Continue") == IOStatus::Continue);
    CHECK(IOStatus::FromString("OK") == IOStatus::OK);
    CHECK(IOStatus::FromString("BadRequest") == IOStatus::BadRequest);
    CHECK(IOStatus::FromString("Forbidden") == IOStatus::Forbidden);
    CHECK(IOStatus::FromString("NotFound") == IOStatus::NotFound);
    CHECK(IOStatus::FromString("MethodNotAllowed") == IOStatus::MethodNotAllowed);
    CHECK(IOStatus::FromString("NotAcceptable") == IOStatus::NotAcceptable);
    CHECK(IOStatus::FromString("InternalServerError") == IOStatus::InternalServerError);
    CHECK(IOStatus::FromString("NotImplemented") == IOStatus::NotImplemented);
    CHECK(IOStatus::FromString("BadGateway") == IOStatus::BadGateway);
    CHECK(IOStatus::FromString("ServiceUnavailable") == IOStatus::ServiceUnavailable);
    CHECK(IOStatus::FromString("Bla") == IOStatus::InvalidIOStatus);
}
