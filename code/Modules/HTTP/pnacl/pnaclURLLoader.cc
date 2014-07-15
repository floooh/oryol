//------------------------------------------------------------------------------
//  pnaclURLLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "pnaclURLLoader.h"
#include "Core/pnacl/pnaclInstance.h"
#include "IO/Stream/MemoryStream.h"
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/url_request_info.h>
#include <ppapi/cpp/url_response_info.h>
#include <ppapi/cpp/url_loader.h>

namespace Oryol {
namespace HTTP {

using namespace Core;
using namespace IO;

// a helper class to wrap all request-related data into a ref-counted object
class pnaclRequestWrapper : public Core::RefCounted {
    OryolClassPoolAllocDecl(pnaclRequestWrapper);
public:
    pnaclRequestWrapper(Ptr<HTTPProtocol::HTTPRequest> req) {
        this->httpRequest = req;
        pp::Instance* ppInst = pnaclInstance::Instance();
        this->ppUrlRequestInfo = pp::URLRequestInfo(ppInst);
        this->ppUrlRequestInfo.SetMethod("GET");
        String urlPath = req->GetURL().PathToEnd();
        this->ppUrlRequestInfo.SetURL(urlPath.AsCStr());
        this->ppUrlLoader = pp::URLLoader(ppInst);
    };
    virtual ~pnaclRequestWrapper() {
        this->httpRequest = nullptr;
        this->ppUrlLoader = pp::URLLoader();
        this->ppUrlRequestInfo = pp::URLRequestInfo();
    }

    void readBodyData() {
        const Ptr<Stream>& responseBody = this->httpRequest->GetResponse()->GetBody();
        if (!responseBody->IsOpen()) {
            responseBody->Open(OpenMode::WriteOnly);
        }
        pp::CompletionCallback cc = pp::CompletionCallback(pnaclURLLoader::cbOnRead, this);
        int32_t result = PP_OK;
        do {
            result = this->ppUrlLoader.ReadResponseBody(this->readBuffer, ReadBufferSize, cc);
            if (result > 0) {
                responseBody->Write(this->readBuffer, result);
            }
        }
        while (result > 0);
        if (PP_OK_COMPLETIONPENDING != result) {
            cc.Run(result);
        }
    }

    Ptr<HTTPProtocol::HTTPRequest> httpRequest;
    pp::URLRequestInfo ppUrlRequestInfo;
    pp::URLLoader ppUrlLoader;
    static const int32 ReadBufferSize = 4096;
    uint8 readBuffer[ReadBufferSize];
};
OryolClassPoolAllocImpl(pnaclRequestWrapper);

//------------------------------------------------------------------------------
void
pnaclURLLoader::doWork() {
    while (!this->requestQueue.Empty()) {
        this->startRequest(this->requestQueue.Dequeue());
    }
}

//------------------------------------------------------------------------------
void
pnaclURLLoader::startRequest(const Ptr<HTTPProtocol::HTTPRequest>& req) {
    o_assert(req.isValid() && !req->Handled());

    // currently only support GET
    o_assert(req->GetMethod() == HTTPMethod::Get);

    // bump the requests refcount and get a raw pointer
    HTTPProtocol::HTTPRequest* reqPtr = req.get();
    reqPtr->addRef();

    // fire off main-thread callback to create and send the HTTP request
    pp::Module::Get()->core()->CallOnMainThread(0, pp::CompletionCallback(cbSendRequest, reqPtr));
}

//------------------------------------------------------------------------------
void
pnaclURLLoader::cbSendRequest(void* data, int32_t /*result*/) {
    o_assert(pp::Module::Get()->core()->IsMainThread());

    // FIXME: support request headers, support methods other then GET,
    // support request body
    Ptr<HTTPProtocol::HTTPRequest> httpRequest = (HTTPProtocol::HTTPRequest*) data;
    auto req = pnaclRequestWrapper::Create(httpRequest); 
    httpRequest->release();
    req->addRef();
    pnaclRequestWrapper* reqPtr = req.get();
    int32_t openResult = req->ppUrlLoader.Open(req->ppUrlRequestInfo, pp::CompletionCallback(cbRequestComplete, reqPtr));
    o_assert(PP_OK_COMPLETIONPENDING == openResult);
}

//------------------------------------------------------------------------------
void
pnaclURLLoader::cbRequestComplete(void* data, int32_t result) {
    o_assert(PP_OK == result);

    Ptr<pnaclRequestWrapper> req((pnaclRequestWrapper*)data);

    // create a response object, and a memory stream object which
    // will hold the received data
    auto httpResponse = HTTPProtocol::HTTPResponse::Create();
    req->httpRequest->SetResponse(httpResponse);
    Ptr<MemoryStream> httpResponseBody = MemoryStream::Create();
    httpResponse->SetBody(httpResponseBody);

    // translate response
    o_assert(!req->ppUrlLoader.is_null());
    o_assert(!req->ppUrlLoader.GetResponseInfo().is_null());
    IOStatus::Code httpStatus = (IOStatus::Code) req->ppUrlLoader.GetResponseInfo().GetStatusCode();
    httpResponse->SetStatus(httpStatus);
    if (httpStatus == IOStatus::OK) {
        // response header received ok, start loading response body,
        // first get the immediately available data now, and maybe
        // do async call to fetch the rest
        req->readBodyData();
    }
    else {
        // HTTP error, dump a warning, and cleanup
        Log::Warn("pnaclURLLoader::cbRequestComplete: GET '%s' returned with '%d'\n", 
            req->httpRequest->GetURL().AsCStr(), httpStatus);
        req->httpRequest->SetHandled();
        req->release();
    }
}

//------------------------------------------------------------------------------
void
pnaclURLLoader::cbOnRead(void* data, int32_t result) {
    Ptr<pnaclRequestWrapper> req((pnaclRequestWrapper*)data);
    if (PP_OK == result)
    {
        // all data received
        req->httpRequest->GetResponse()->GetBody()->Close();
        req->httpRequest->SetHandled();
        req->release();
        return;
    }
    else if (result > 0)
    {
        // read all available data...
        req->httpRequest->GetResponse()->GetBody()->Write(req->readBuffer, result);
        req->readBodyData();
    }
    else
    {
        // an error occured
        Log::Warn("pnaclURLLoader::cbOnRead: Error while reading body data.\n");
        req->httpRequest->GetResponse()->SetStatus(IOStatus::DownloadError);
        req->httpRequest->SetHandled();
        req->release();
        return;
    }
}

} // namespace HTTP
} // namespace Oryol