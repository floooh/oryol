//------------------------------------------------------------------------------
//  HTTPMethodTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "HTTP/HTTPMethod.h"
#include "Core/String/String.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::HTTP;

TEST(HTTPMethodTest) {
    
    // check to-string converion
    CHECK(String(HTTPMethod::ToString(HTTPMethod::OPTIONS)) == "OPTIONS");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::GET)) == "GET");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::HEAD)) == "HEAD");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::POST)) == "POST");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::PUT)) == "PUT");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::DELETE)) == "DELETE");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::TRACE)) == "TRACE");
    CHECK(String(HTTPMethod::ToString(HTTPMethod::CONNECT)) == "CONNECT");
    CHECK(String(HTTPMethod::ToString((HTTPMethod::Code)123)) == "InvalidHTTPMethod");
    
    // check from-string conversion
    CHECK(HTTPMethod::FromString("OPTIONS") == HTTPMethod::OPTIONS);
    CHECK(HTTPMethod::FromString("GET") == HTTPMethod::GET);
    CHECK(HTTPMethod::FromString("HEAD") == HTTPMethod::HEAD);
    CHECK(HTTPMethod::FromString("POST") == HTTPMethod::POST);
    CHECK(HTTPMethod::FromString("PUT") == HTTPMethod::PUT);
    CHECK(HTTPMethod::FromString("DELETE") == HTTPMethod::DELETE);
    CHECK(HTTPMethod::FromString("TRACE") == HTTPMethod::TRACE);
    CHECK(HTTPMethod::FromString("CONNECT") == HTTPMethod::CONNECT);
    CHECK(HTTPMethod::FromString("Bla") == HTTPMethod::InvalidHTTPMethod);
}
