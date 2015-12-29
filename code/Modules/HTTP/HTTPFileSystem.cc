//------------------------------------------------------------------------------
//  HTTPFileSystem.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "HTTPFileSystem.h"

namespace Oryol {
    
OryolClassImpl(HTTPFileSystem);

//------------------------------------------------------------------------------
void
HTTPFileSystem::InitLane() {
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
Ptr<HTTPProtocol::HTTPRequest>
HTTPFileSystem::createHttpRequest(HTTPMethod::Code method, const Ptr<IOProtocol::Request>& ioReq) {
    Ptr<HTTPProtocol::HTTPRequest> httpReq = HTTPProtocol::HTTPRequest::Create();
    httpReq->Method = method;
    httpReq->Url = ioReq->Url;
    httpReq->Type = ioReq->Type;
    httpReq->Body = std::move(ioReq->Data);
    httpReq->IoRequest = ioReq;
    if ((ioReq->StartOffset != 0) || (ioReq->EndOffset != EndOfFile)) {
        o_error("FIXME: implement HTTP Range requests!\n");
    }
    else {
        httpReq->RequestHeaders = this->requestHeaders;
    }
    return httpReq;
}

//------------------------------------------------------------------------------
void
HTTPFileSystem::onRead(const Ptr<IOProtocol::Read>& ioReq) {
    this->httpClient->Put(this->createHttpRequest(HTTPMethod::Get, ioReq));
}

//------------------------------------------------------------------------------
void
HTTPFileSystem::onWrite(const Ptr<IOProtocol::Write>& ioReq) {
    this->httpClient->Put(this->createHttpRequest(HTTPMethod::Post, ioReq));
}

} // namespace Oryol
