//------------------------------------------------------------------------------
//  HTTPMethodTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "HTTP/HTTPMethod.h"
#include "Core/String/String.h"

using namespace Oryol;
using namespace Oryol::HTTP;

TEST(HTTPMethodTest) {
    
    // check to-string converion
    CHECK(String(HTTPMethod::ToString(HTTPMethod::Options)) == "Options");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::Get)) == "Get");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::Head)) == "Head");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::Post)) == "Post");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::Put)) == "Put");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::Delete)) == "Delete");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::Trace)) == "Trace");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::Connect)) == "Connect");
    CHECK(String(HTTPMethod::ToString((HTTPMethod::Code)123)) == "InvalidHTTPMethod");
    
    // check from-string conversion
    CHECK(HTTPMethod::FromString("Options") == HTTPMethod::Options);
    CHECK(HTTPMethod::FromString("Get") == HTTPMethod::Get);
    CHECK(HTTPMethod::FromString("Head") == HTTPMethod::Head);
    CHECK(HTTPMethod::FromString("Post") == HTTPMethod::Post);
    CHECK(HTTPMethod::FromString("Put") == HTTPMethod::Put);
    CHECK(HTTPMethod::FromString("Delete") == HTTPMethod::Delete);
    CHECK(HTTPMethod::FromString("Trace") == HTTPMethod::Trace);
    CHECK(HTTPMethod::FromString("Connect") == HTTPMethod::Connect);
    CHECK(HTTPMethod::FromString("Bla") == HTTPMethod::InvalidHTTPMethod);
}
