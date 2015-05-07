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

    // add standard request headers:
    //  User-Agent: need a 'standard' user-agent, otherwise some HTTP servers
    //              won't accept Connection: keep-alive
    //  Connection: keep-alive, don't open/close the connection all the time
    //  Accept-Encoding:    gzip, deflate
    //
    this->requestHeaders.Add("User-Agent", "Mozilla/5.0");
    this->requestHeaders.Add("Connection", "keep-alive");
    this->requestHeaders.Add("Accept-Encoding", "gzip, deflate");
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
            Map<String,String> reqHeaders = this->requestHeaders;
            // need to add a Range header
            this->stringBuilder.Format(64, "bytes=%d-%d", msg->GetStartOffset(), msg->GetEndOffset());
            reqHeaders.Add("Range", this->stringBuilder.GetString());
            httpReq->SetRequestHeaders(reqHeaders);
        }
        else {
            httpReq->SetRequestHeaders(this->requestHeaders);
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
