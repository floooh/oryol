//------------------------------------------------------------------------------
//  HTTPFileSystem.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "HTTPFileSystem.h"

namespace Oryol {
namespace HTTP {
    
OryolClassImpl(HTTPFileSystem);

using namespace IO;

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
HTTPFileSystem::onGet(const Ptr<IOProtocol::Get>& msg) {

    // convert the IO request into a HTTP request and push to HTTPClient
    Ptr<HTTPProtocol::HTTPRequest> httpReq = HTTPProtocol::HTTPRequest::Create();
    httpReq->SetMethod(HTTPMethod::Get);
    httpReq->SetURL(msg->GetURL());
    this->httpClient->Put(httpReq);
    
    // add to pending requests
    pendingRequest pending;
    pending.ioRequest = msg;
    pending.httpRequest = httpReq;
    this->pendingRequests.Add(pending);
}

//------------------------------------------------------------------------------
void
HTTPFileSystem::onGetRange(const Ptr<IOProtocol::GetRange>& msg) {
    
    // need to add a Range header
    this->stringBuilder.Format(64, "bytes=%d-%d", msg->GetStartOffset(), msg->GetEndOffset());
    Map<String,String> requestHeaders;
    requestHeaders.Add("Range", this->stringBuilder.GetString());
    
    // convert the IO request into a HTTP request and push to HTTPClient
    Ptr<HTTPProtocol::HTTPRequest> httpReq = HTTPProtocol::HTTPRequest::Create();
    httpReq->SetMethod(HTTPMethod::Get);
    httpReq->SetURL(msg->GetURL());
    httpReq->SetRequestHeaders(requestHeaders);
    this->httpClient->Put(httpReq);
    
    // add to pending requests
    pendingRequest pending;
    pending.ioRequest = msg;
    pending.httpRequest = httpReq;
    this->pendingRequests.Add(pending);
}

//------------------------------------------------------------------------------
void
HTTPFileSystem::DoWork() {
    
    // trigger our http client
    this->httpClient->DoWork();
    
    // process pending requests
    for (int i = this->pendingRequests.Size() - 1; i >= 0; i--) {
        if (this->pendingRequests[i].httpRequest->Handled()) {
            // ok this request is done, transfer the interesting
            // stuff over to the ioRequest
            const pendingRequest& cur = this->pendingRequests[i];
            const Ptr<HTTPProtocol::HTTPResponse>& httpResponse = cur.httpRequest->GetResponse();
            cur.ioRequest->SetStatus(httpResponse->GetStatus());
            cur.ioRequest->SetStream(httpResponse->GetBody());
            cur.ioRequest->SetErrorDesc(httpResponse->GetErrorDesc());
            cur.ioRequest->SetHandled();
            
            // of this request is done, remove from pending array
            this->pendingRequests.Erase(i);
        }
    }
}
    
} // namespace HTTP
} // namespace Oryol