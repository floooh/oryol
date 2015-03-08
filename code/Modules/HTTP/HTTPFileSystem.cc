//------------------------------------------------------------------------------
//  HTTPFileSystem.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "HTTPFileSystem.h"

namespace Oryol {
    
OryolClassImpl(HTTPFileSystem);

//------------------------------------------------------------------------------
HTTPFileSystem::HTTPFileSystem() {
    this->httpClient = HTTPClient::Create();
}

//------------------------------------------------------------------------------
HTTPFileSystem::~HTTPFileSystem() {
    // empty
}

//------------------------------------------------------------------------------
void
HTTPFileSystem::onRequest(const Ptr<IOProtocol::Request>& msg) {

    // convert the IO request into a HTTP request and push to HTTPClient
    if (!msg->Cancelled()) {
        Ptr<HTTPProtocol::HTTPRequest> httpReq = HTTPProtocol::HTTPRequest::Create();
        httpReq->SetMethod(HTTPMethod::Get);
        httpReq->SetURL(msg->GetURL());
        httpReq->SetIoRequest(msg);
        if (msg->GetEndOffset() != 0) {
            Map<String,String> requestHeaders;
            // need to add a Range header
            this->stringBuilder.Format(64, "bytes=%d-%d", msg->GetStartOffset(), msg->GetEndOffset());
            requestHeaders.Add("Range", this->stringBuilder.GetString()); 
            httpReq->SetRequestHeaders(requestHeaders);
        }
        this->httpClient->Put(httpReq);
    }
}

//------------------------------------------------------------------------------
void
HTTPFileSystem::DoWork() {
    
    // trigger our http client
    this->httpClient->DoWork();
}
    
} // namespace Oryol
