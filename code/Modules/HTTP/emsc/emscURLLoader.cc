//------------------------------------------------------------------------------
//  emscURLLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "emscURLLoader.h"
#include "IO/MemoryStream.h"
#include <emscripten/emscripten.h>

namespace Oryol {
namespace HTTP {

using namespace Core;
using namespace IO;

//------------------------------------------------------------------------------
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
    o_assert(req->GetMethod() == HTTPMethod::Get);

    // bump the requests refcount and get a raw pointer
    HTTPProtocol::HTTPRequest* reqPtr = req.get();
    reqPtr->addRef();

    // start the asynchronous XHR
    // NOTE: we can only load from our own HTTP server, so the host part of 
    // the URL is completely irrelevant...
    String urlPath = req->GetURL().PathToEnd().AsCStr();
    Log::Dbg("emscURLLoader::startRequest(): %s\n", urlPath.AsCStr());
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
    Log::Dbg("emscURLLoader::onLoaded(url=%s, size=%d)\n", req->GetURL().AsCStr(), size);

    // create a HTTPResponse and fill it out
    Ptr<HTTPProtocol::HTTPResponse> response = HTTPProtocol::HTTPResponse::Create();
    response->SetStatus(IOStatus::OK);

    // create a MemoryStream object and fill it with the received data
    Ptr<MemoryStream> responseBody = MemoryStream::Create();
    responseBody->Open(OpenMode::WriteOnly);
    responseBody->Write(buffer, size);
    responseBody->Close();
    response->SetBody(responseBody);

    // set the response on the request
    req->SetResponse(response);

    // and we're done here!
    req->SetHandled();

    // this fixes the ref-count we had to increase earlier because a raw
    // pointer was involved, we better only decrement the refcount again
    // here at the end, just to make really sure that the method body worked
    // on a valid object (hmm ok, shouldn't be necessary)
    req->release();
}

//------------------------------------------------------------------------------
void
emscURLLoader::onFailed(void* userData) {
    o_assert(0 != userData);

    // user data is a HTTPRequest ptr, put it back into a smart pointer
    Ptr<HTTPProtocol::HTTPRequest> req = userData;
    Log::Dbg("emscURLLoader::onFailed(url=%s)\n", req->GetURL().AsCStr());

    // hmm we don't know why it failed, so make something up, we should definitely
    // fix this somehow (looks like the wget2 functions also pass a HTTP status code)
    Ptr<HTTPProtocol::HTTPResponse> response = HTTPProtocol::HTTPResponse::Create();
    response->SetStatus(IOStatus::NotFound);
    req->SetResponse(response);

    // and we're done, see onLoaded()
    req->SetHandled();
    req->release();
}

} // namespace HTTP
} // naespace Oryol