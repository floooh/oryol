//------------------------------------------------------------------------------
//  curlURLLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "curlURLLoader.h"
#include "IO/Stream/MemoryStream.h"
#include "curl/curl.h"

#if LIBCURL_VERSION_NUM != 0x072400
#error "Not using the right curl version, header search path fuckup?"
#endif

namespace Oryol {
namespace HTTP {

using namespace Core;
using namespace IO;

bool curlURLLoader::curlInitCalled = false;
std::mutex curlURLLoader::curlInitMutex;

//------------------------------------------------------------------------------
curlURLLoader::curlURLLoader() :
contentTypeString("Content-Type"),
curlSession(0),
curlError(0) {

    // we need to do some one-time curl initialization here,
    // thread-protected because curl_global_init() is not thread-safe
    curlInitMutex.lock();
    if (!curlInitCalled) {
        CURLcode curlInitRes = curl_global_init(CURL_GLOBAL_ALL);
        o_assert(0 == curlInitRes);
        curlInitCalled = true;
    }
    curlInitMutex.unlock();

    // setup a new curl session
    this->setupCurlSession();
}

//------------------------------------------------------------------------------
curlURLLoader::~curlURLLoader() {
    this->discardCurlSession();
}

//------------------------------------------------------------------------------
void
curlURLLoader::setupCurlSession() {
    o_assert(0 == this->curlError);
    o_assert(0 == this->curlSession);

    // setup the error buffer
    const int32 curlErrorBufferSize = CURL_ERROR_SIZE * 4;
    this->curlError = (char*) Memory::Alloc(curlErrorBufferSize);
    Memory::Clear(this->curlError, curlErrorBufferSize);

    // setup the curl session
    this->curlSession = curl_easy_init();
    o_assert(0 != this->curlSession);

    // set session options
    curl_easy_setopt(this->curlSession, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(this->curlSession, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(this->curlSession, CURLOPT_ERRORBUFFER, this->curlError);
    curl_easy_setopt(this->curlSession, CURLOPT_WRITEFUNCTION, curlWriteDataCallback);
    curl_easy_setopt(this->curlSession, CURLOPT_HEADERFUNCTION, curlHeaderCallback);
    curl_easy_setopt(this->curlSession, CURLOPT_WRITEHEADER, this);
    curl_easy_setopt(this->curlSession, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(this->curlSession, CURLOPT_TCP_KEEPIDLE, 10L);
    curl_easy_setopt(this->curlSession, CURLOPT_TCP_KEEPINTVL, 10L);
}

//------------------------------------------------------------------------------
void
curlURLLoader::discardCurlSession() {
    o_assert(0 != this->curlError);
    o_assert(0 != this->curlSession);

    curl_easy_cleanup(this->curlSession);
    this->curlSession = 0;
    Memory::Free(this->curlError);
    this->curlError = 0;
}

//------------------------------------------------------------------------------
size_t
curlURLLoader::curlWriteDataCallback(char* ptr, size_t size, size_t nmemb, void* userData) {
    // userData is expected to point to a Stream object, open for writing
    int32 bytesToWrite = (int32) (size * nmemb);
    if (bytesToWrite > 0) {
        Stream* stream = (Stream*) userData;
        stream->Write(ptr, bytesToWrite);
        return bytesToWrite;
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
size_t
curlURLLoader::curlHeaderCallback(char* ptr, size_t size, size_t nmemb, void* userData) {
    // userData is expected to point to the curlURLLoader object
    curlURLLoader* self = (curlURLLoader*) userData;
    int32 receivedBytes = (int32) (size * nmemb);
    if (receivedBytes > 0) {
        self->stringBuilder.Set(ptr, 0, receivedBytes);
        int32 colonIndex = self->stringBuilder.FindFirstOf(0, receivedBytes, ":");
        if (InvalidIndex != colonIndex) {
            String key = self->stringBuilder.GetSubString(0, colonIndex);
            int32 endOfValueIndex = self->stringBuilder.FindFirstOf(colonIndex, EndOfString, "\r\n");
            String value = self->stringBuilder.GetSubString(colonIndex + 2, endOfValueIndex);
            self->responseHeaders.Insert(key, value);
        }
        return receivedBytes;
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
void
curlURLLoader::doWork() {
    while (!this->requestQueue.Empty()) {
        Ptr<HTTPProtocol::HTTPRequest> req = this->requestQueue.Dequeue();
        this->doOneRequest(req);
        req->SetHandled();
    }
}

//------------------------------------------------------------------------------
void
curlURLLoader::doOneRequest(const Ptr<HTTPProtocol::HTTPRequest>& req) {
    o_assert(0 != this->curlSession);
    o_assert(0 != this->curlError);
    this->stringBuilder.Clear();
    this->responseHeaders.Clear();

    // set URL in curl
    const URL& url = req->GetURL();
    o_assert(url.Scheme() == "http");
    curl_easy_setopt(this->curlSession, CURLOPT_URL, url.AsCStr());

    // set the HTTP method
    /// @todo: only HTTP GET and POST supported for now
    switch (req->GetMethod()) {
        case HTTPMethod::Get:  curl_easy_setopt(this->curlSession, CURLOPT_HTTPGET, 1); break;
        case HTTPMethod::Post: curl_easy_setopt(this->curlSession, CURLOPT_POST, 1); break;
        default: o_error("curlURLLoader: unsupported HTTP method '%s'\n", HTTPMethod::ToString(req->GetMethod())); break;
    }

    // setup request header fields
    struct curl_slist* requestHeaders = 0;
    for (const auto& kvp : req->GetRequestHeaders()) {
        this->stringBuilder.Set(kvp.Key());
        this->stringBuilder.Append(": ");
        this->stringBuilder.Append(kvp.Value());
        requestHeaders = curl_slist_append(requestHeaders, this->stringBuilder.AsCStr());
    }

    // if this is a POST, set the data to post
    const Ptr<Stream>& postStream = req->GetBody();
    if (req->GetMethod() == HTTPMethod::Post) {
        o_assert(postStream.isValid());
        postStream->Open(OpenMode::ReadOnly);
        const uint8* endPtr = nullptr;
        const uint8* postData = postStream->MapRead(&endPtr);
        const int32 postDataSize = postStream->Size();
        o_assert((endPtr - postData) == postDataSize);
        curl_easy_setopt(this->curlSession, CURLOPT_POSTFIELDS, postData);
        curl_easy_setopt(this->curlSession, CURLOPT_POSTFIELDSIZE, postDataSize);

        // add a Content-Type request header if the post-stream has a content-type set
        if (postStream->GetContentType().IsValid()) {
            this->stringBuilder.Set("Content-Type: ");
            this->stringBuilder.Append(postStream->GetContentType().AsCStr());
            requestHeaders = curl_slist_append(requestHeaders, this->stringBuilder.AsCStr());
        }
    }

    // set the http request headers
    if (0 != requestHeaders) {
        curl_easy_setopt(this->curlSession, CURLOPT_HTTPHEADER, requestHeaders);
    }

    // prepare the HTTPResponse and the response-body stream
    Ptr<HTTPProtocol::HTTPResponse> httpResponse = HTTPProtocol::HTTPResponse::Create();
    Ptr<MemoryStream> responseBodyStream = MemoryStream::Create();
    responseBodyStream->Open(OpenMode::WriteOnly);
    curl_easy_setopt(this->curlSession, CURLOPT_WRITEDATA, responseBodyStream.get());

    // perform the request
    CURLcode performResult = curl_easy_perform(this->curlSession);

    // query the http code
    long curlHttpCode = 0;
    curl_easy_getinfo(this->curlSession, CURLINFO_RESPONSE_CODE, &curlHttpCode);
    httpResponse->SetStatus((IOStatus::Code) curlHttpCode);

    // check for error codes
    if (CURLE_PARTIAL_FILE == performResult) {
        // this seems to happen quite often even though all data has been received,
        // not sure what to do about this, but don't treat it as an error
        Log::Warn("curlURLLoader: CURLE_PARTIAL_FILE received for '%s', httpStatus='%ld'\n", req->GetURL().AsCStr(), curlHttpCode);
        httpResponse->SetErrorDesc(this->curlError);
    }
    else if (0 != performResult) {
        // some other curl error
        Log::Warn("curlURLLoader: curl_easy_peform failed with '%s' for '%s', httpStatus='%ld'\n",
            req->GetURL().AsCStr(), this->curlError, curlHttpCode);
        httpResponse->SetErrorDesc(this->curlError);
    }

    // check if the responseHeaders contained a Content-Type, if yes, set it on the responseBodyStream
    if (this->responseHeaders.Contains(this->contentTypeString)) {
        responseBodyStream->SetContentType(this->responseHeaders[this->contentTypeString]);
    }

    // close the responseBodyStream, and set the result
    responseBodyStream->Close();
    httpResponse->SetResponseHeaders(this->responseHeaders);
    httpResponse->SetBody(responseBodyStream);
    req->SetResponse(httpResponse);

    // close the optional body stream
    if (postStream.isValid() && postStream->IsOpen()) {
        postStream->Close();
    }

    // free the previously allocated request headers
    if (0 != requestHeaders) {
        curl_slist_free_all(requestHeaders);
        requestHeaders = 0;
    }
}

} // namespace HTTP
} // namespace Oryol
