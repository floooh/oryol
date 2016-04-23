//------------------------------------------------------------------------------
//  pnaclURLLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "pnaclURLLoader.h"
#include "Core/pnacl/pnaclInstance.h"
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/url_request_info.h>
#include <ppapi/cpp/url_response_info.h>
#include <ppapi/cpp/url_loader.h>

namespace Oryol {
namespace _priv {

// a helper class to wrap all request-related data into a ref-counted object
class pnaclRequestWrapper : public RefCounted {
    OryolClassDecl(pnaclRequestWrapper);
public:
    pnaclRequestWrapper(Ptr<IORead> req) {
        this->ioRequest = req;
        pp::Instance* ppInst = pnaclInstance::Instance();
        this->ppUrlRequestInfo = pp::URLRequestInfo(ppInst);
        this->ppUrlRequestInfo.SetMethod("GET");
        String urlPath = req->Url.PathToEnd();
        this->ppUrlRequestInfo.SetURL(urlPath.AsCStr());
        this->ppUrlLoader = pp::URLLoader(ppInst);
    };
    virtual ~pnaclRequestWrapper() {
        this->ioRequest = nullptr;
        this->ppUrlLoader = pp::URLLoader();
        this->ppUrlRequestInfo = pp::URLRequestInfo();
    }

    void readBodyData() {
        auto& responseBody = this->ioRequest->Data;
        pp::CompletionCallback cc = pp::CompletionCallback(pnaclURLLoader::cbOnRead, this);
        int32_t result = PP_OK;
        do {
            result = this->ppUrlLoader.ReadResponseBody(this->readBuffer, ReadBufferSize, cc);
            if (result > 0) {
                responseBody.Add(this->readBuffer, result);
            }
        }
        while (result > 0);
        if (PP_OK_COMPLETIONPENDING != result) {
            cc.Run(result);
        }
    }

    Ptr<IORead> ioRequest;
    pp::URLRequestInfo ppUrlRequestInfo;
    pp::URLLoader ppUrlLoader;
    static const int32 ReadBufferSize = 4096;
    uint8 readBuffer[ReadBufferSize];
};

//------------------------------------------------------------------------------
bool
pnaclURLLoader::doRequest(const Ptr<IORead>& ioReq) {
    if (baseURLLoader::doRequest(ioReq)) {
        this->startRequest(ioReq);
        return true;
    }
    else {
        // request was cancelled
        return false;
    }
}

//------------------------------------------------------------------------------
void
pnaclURLLoader::startRequest(const Ptr<IORead>& req) {
    o_assert(req.isValid() && !req->Handled);

    // bump the requests refcount and get a raw pointer
    IORead* reqPtr = req.get();
    reqPtr->addRef();

    // fire off main-thread callback to create and send the HTTP request
    pp::Module::Get()->core()->CallOnMainThread(0, pp::CompletionCallback(cbSendRequest, reqPtr));
}

//------------------------------------------------------------------------------
void
pnaclURLLoader::cbSendRequest(void* data, int32_t /*result*/) {
    o_assert(pp::Module::Get()->core()->IsMainThread());

    Ptr<IORead> ioReq = (IORead*) data;
    auto req = pnaclRequestWrapper::Create(ioReq); 
    ioReq->release();
    pnaclRequestWrapper* reqPtr = req.get();
    reqPtr->addRef();
    ORYOL_UNUSED int32_t openResult = req->ppUrlLoader.Open(req->ppUrlRequestInfo, pp::CompletionCallback(cbRequestComplete, reqPtr));
    o_assert(PP_OK_COMPLETIONPENDING == openResult);
}

//------------------------------------------------------------------------------
void
pnaclURLLoader::cbRequestComplete(void* data, int32_t result) {
    o_assert(PP_OK == result);

    Ptr<pnaclRequestWrapper> req((pnaclRequestWrapper*)data);

    // translate response
    o_assert(!req->ppUrlLoader.is_null());
    o_assert(!req->ppUrlLoader.GetResponseInfo().is_null());
    IOStatus::Code httpStatus = (IOStatus::Code) req->ppUrlLoader.GetResponseInfo().GetStatusCode();
    req->ioRequest->Status = httpStatus;
    if (httpStatus == IOStatus::OK) {
        // response header received ok, start loading response body,
        // first get the immediately available data now, and maybe
        // do async call to fetch the rest
        req->readBodyData();
        // NOTE: do not release since cbOnRead still needs the pnaclRequestWrapper!
    }
    else {
        // HTTP error, dump a warning, and cleanup
        Log::Warn("pnaclURLLoader::cbRequestComplete: GET '%s' returned with '%d'\n", 
            req->ioRequest->Url.AsCStr(), httpStatus);
        req->ioRequest->Handled = true;
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
        req->ioRequest->Handled = true;
        req->release();
    }
    else if (result > 0)
    {
        // read all available data..., do not release the pnaclRequestWrapper
        // since it is still needed in the following callbacks
        req->ioRequest->Data.Add(req->readBuffer, result);
        req->readBodyData();
    }
    else
    {
        // an error occurred
        Log::Warn("pnaclURLLoader::cbOnRead: Error while reading body data.\n");
        req->ioRequest->Status = IOStatus::DownloadError;
        req->ioRequest->Handled = true;
        req->release();
    }
}

} // namespace _priv
} // namespace Oryol

