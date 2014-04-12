//------------------------------------------------------------------------------
//  HTTPClientTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "HTTP/HTTPClient.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::IO;
using namespace Oryol::HTTP;

#if !ORYOL_EMSCRIPTEN
TEST(HTTPClientTest) {

    // create a couple of requests
    Ptr<HTTPProtocol::HTTPRequest> reqOk = HTTPProtocol::HTTPRequest::Create();
    CHECK(reqOk->GetMethod() == HTTPMethod::Get);
    reqOk->SetURL("http://www.flohofwoe.net/index.html");
    Ptr<HTTPProtocol::HTTPRequest> req404 = HTTPProtocol::HTTPRequest::Create();
    req404->SetURL("http://www.google.com/blargh");

    // create a HTTP client and fire the request
    Ptr<HTTPClient> httpClient = HTTPClient::Create();
    httpClient->Put(reqOk);
    httpClient->Put(req404);
    while (!(reqOk->Handled() && req404->Handled())) {
        httpClient->DoWork();
    }
    
    // check valid response
    CHECK(reqOk->GetResponse().isValid());
    CHECK(reqOk->GetResponse()->GetStatus() == IOStatus::OK);
    const Map<String, String>& fields = reqOk->GetResponse()->GetResponseHeaders();
    CHECK(fields.Contains("Content-Type"));
    CHECK(fields["Content-Type"] == "text/html");
    CHECK(reqOk->GetResponse()->GetBody().isValid());
    CHECK(reqOk->GetResponse()->GetBody()->Size() > 500);
    CHECK(reqOk->GetResponse()->GetBody()->GetContentType().TypeAndSubType() == "text/html");
    
    // check 404 response
    CHECK(req404->GetResponse().isValid());
    CHECK(req404->GetResponse()->GetStatus() == IOStatus::NotFound);
}
#endif

