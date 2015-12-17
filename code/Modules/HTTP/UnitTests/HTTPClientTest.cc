//------------------------------------------------------------------------------
//  HTTPClientTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "HTTP/HTTPClient.h"

using namespace Oryol;

#if !ORYOL_EMSCRIPTEN
TEST(HTTPClientTest) {

    // create a couple of requests
    Ptr<HTTPProtocol::HTTPRequest> reqOk = HTTPProtocol::HTTPRequest::Create();
    CHECK(reqOk->Method == HTTPMethod::Get);
    reqOk->Url = "http://www.flohofwoe.net/index.html";
    Ptr<HTTPProtocol::HTTPRequest> req404 = HTTPProtocol::HTTPRequest::Create();
    req404->Url = "http://www.google.com/blargh";

    // create a HTTP client and fire the request
    Ptr<HTTPClient> httpClient = HTTPClient::Create();
    httpClient->Put(reqOk);
    httpClient->Put(req404);
    while (!(reqOk->Handled() && req404->Handled())) {
        httpClient->DoWork();
    }
    
    // check valid response
    CHECK(reqOk->Response.isValid());
    CHECK(reqOk->Response->Status == IOStatus::OK);
    const Map<String, String>& fields = reqOk->Response->ResponseHeaders;
    CHECK(fields.Contains("Content-Type"));
    CHECK(fields["Content-Type"] == "text/html");
    CHECK(!reqOk->Response->Body.Empty());
    CHECK(reqOk->Response->Body.Size() > 500);
    CHECK(reqOk->Response->Type.TypeAndSubType() == "text/html");
    
    // check 404 response
    CHECK(req404->Response.isValid());
    CHECK(req404->Response->Status == IOStatus::NotFound);
}
#endif

