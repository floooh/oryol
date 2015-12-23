//------------------------------------------------------------------------------
//  emscURLLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "emscURLLoader.h"
#include <emscripten/emscripten.h>

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
// FIXME FIXME FIXME
// Properly handle cancelled messages.
void
emscURLLoader::doWork() {
    while (!this->requestQueue.Empty()) {
        this->startRequest(this->requestQueue.Dequeue());
    }
}

//------------------------------------------------------------------------------
void
emscURLLoader::startRequest(const Ptr<HTTPProtocol::HTTPRequest>& req) {
    o_assert(req.isValid() && !req->Handled());

    // currently only support GET
    o_assert(HTTPMethod::Get == req->Method);

    // bump the requests refcount and get a raw pointer
    HTTPProtocol::HTTPRequest* reqPtr = req.get();
    reqPtr->addRef();

    // start the asynchronous XHR
    // NOTE: we can only load from our own HTTP server, so the host part of 
    // the URL is completely irrelevant...
    String urlPath = req->Url.PathToEnd();
    emscripten_async_wget_data(urlPath.AsCStr(), (void*) reqPtr, emscURLLoader::onLoaded, emscURLLoader::onFailed);
}

//------------------------------------------------------------------------------
void
emscURLLoader::onLoaded(void* userData, void* buffer, int size) {
    o_assert(0 != userData);
    o_assert(0 != buffer);
    o_assert(size > 0);

    // user data is a HTTPRequest ptr, put it back into a smart pointer
    Ptr<HTTPProtocol::HTTPRequest> req = userData;
    req->release();

    // create a HTTPResponse and fill it out
    Ptr<HTTPProtocol::HTTPResponse> response = HTTPProtocol::HTTPResponse::Create();
    response->Status = IOStatus::OK;

    // write response body
    const Ptr<IOProtocol::Request>& ioReq = req->IoRequest;
    response->Body.Add((const uint8*)buffer, size);

    // set the response on the request, mark the request as handled
    // also fill the embedded IORequest object
    req->Response = response;
    if (ioReq) {
        auto httpResponse = req->Response;
        ioReq->Status = httpResponse->Status;
        ioReq->Data = std::move(httpResponse->Body);
        ioReq->ErrorDesc = httpResponse->ErrorDesc;
        ioReq->SetHandled();
    }
    req->SetHandled();
}

//------------------------------------------------------------------------------
void
emscURLLoader::onFailed(void* userData) {
    o_assert(0 != userData);

    // user data is a HTTPRequest ptr, put it back into a smart pointer
    Ptr<HTTPProtocol::HTTPRequest> req = userData;
    req->release();
    Log::Dbg("emscURLLoader::onFailed(url=%s)\n", req->Url.AsCStr());

    // hmm we don't know why it failed, so make something up, we should definitely
    // fix this somehow (looks like the wget2 functions also pass a HTTP status code)
    const IOStatus::Code ioStatus = IOStatus::NotFound;
    Ptr<HTTPProtocol::HTTPResponse> response = HTTPProtocol::HTTPResponse::Create();
    response->Status = ioStatus;
    req->Response = response;
    auto ioReq = req->IoRequest;
    if (ioReq) {
        auto httpResponse = req->Response;
        ioReq->Status = httpResponse->Status;
        ioReq->SetHandled();
    }
    req->SetHandled();
}

} // namespace _priv
} // naespace Oryol

