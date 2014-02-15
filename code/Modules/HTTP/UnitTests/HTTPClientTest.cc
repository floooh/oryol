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

TEST(HTTPClientTest) {

    // create a GET request to www.flohofwoe.net
    Ptr<HTTPProtocol::HTTPRequest> req = HTTPProtocol::HTTPRequest::Create();
    req->SetMethod(HTTPMethod::GET);
    req->SetURL("http://www.flohofwoe.net/index.html");

    // create a HTTP client and fire the request
    Ptr<HTTPClient> httpClient = HTTPClient::Create();
    httpClient->Put(req);
    while (!req->Handled()) {
        httpClient->DoWork();
    }
    
    // check response
    CHECK(req->GetResponse().isValid());
    CHECK(req->GetResponse()->GetStatus() == IOStatus::OK);
    const Map<String, String>& fields = req->GetResponse()->GetFields();
    CHECK(fields.Contains("Content-Type"));
    CHECK(fields["Content-Type"] == "text/html");
    CHECK(req->GetResponse()->GetBody().isValid());
    CHECK(req->GetResponse()->GetBody()->Size() > 500);
}
