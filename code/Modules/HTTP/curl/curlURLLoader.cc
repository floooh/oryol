//------------------------------------------------------------------------------
//  curlURLLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "curlURLLoader.h"
#include "Core/String/StringConverter.h"
#include "Core/Containers/Buffer.h"
#include "curl/curl.h"

#if LIBCURL_VERSION_NUM != 0x072400
#error "Not using the right curl version, header search path fuckup?"
#endif

namespace Oryol {
namespace _priv {

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
    curl_easy_setopt(this->curlSession, CURLOPT_TIMEOUT, 30);
    curl_easy_setopt(this->curlSession, CURLOPT_CONNECTTIMEOUT, 30);
    curl_easy_setopt(this->curlSession, CURLOPT_ACCEPT_ENCODING, "");   // all encodings supported by curl
    curl_easy_setopt(this->curlSession, CURLOPT_FOLLOWLOCATION, 1);
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
    // userData is expected to point to a Buffer object
    int32 bytesToWrite = (int32) (size * nmemb);
    if (bytesToWrite > 0) {
        Buffer* buf = (Buffer*) userData;
        buf->Add((const uint8*)ptr, bytesToWrite);
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
            self->responseHeaders.Add(key, value);
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
        if (!baseURLLoader::handleCancelled(req)) {
            this->doOneRequest(req);

            // transfer result to embedded IoRequest object
            auto ioReq = req->IoRequest;
            if (ioReq) {
                auto httpResponse = req->Response;
                ioReq->Status = httpResponse->Status;
                ioReq->Data = std::move(httpResponse->Body);
                ioReq->Type = httpResponse->Type;
                ioReq->ErrorDesc = httpResponse->ErrorDesc;
                ioReq->SetHandled();
            }
            req->SetHandled();
        }
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
    const URL& url = req->Url;
    o_assert(url.Scheme() == "http");
    curl_easy_setopt(this->curlSession, CURLOPT_URL, url.AsCStr());
    if (url.HasPort()) {
        uint16 port = StringConverter::FromString<uint16>(url.Port());
        curl_easy_setopt(this->curlSession, CURLOPT_PORT, port);
    }

    // set the HTTP method
    /// @todo: only HTTP GET and POST supported for now
    switch (req->Method) {
        case HTTPMethod::Get:  curl_easy_setopt(this->curlSession, CURLOPT_HTTPGET, 1); break;
        case HTTPMethod::Post: curl_easy_setopt(this->curlSession, CURLOPT_POST, 1); break;
        default: o_error("curlURLLoader: unsupported HTTP method '%s'\n", HTTPMethod::ToString(req->Method)); break;
    }

    // setup request header fields
    struct curl_slist* requestHeaders = 0;
    for (const auto& kvp : req->RequestHeaders) {
        this->stringBuilder.Set(kvp.Key());
        this->stringBuilder.Append(": ");
        this->stringBuilder.Append(kvp.Value());
        requestHeaders = curl_slist_append(requestHeaders, this->stringBuilder.AsCStr());
    }

    // if this is a POST, set the data to post
    if (req->Method == HTTPMethod::Post) {
        const int32 postDataSize = req->Body.Size();    // can be 0
        const uint8* postData = req->Body.Empty() ? nullptr : req->Body.Data();
        curl_easy_setopt(this->curlSession, CURLOPT_POSTFIELDS, postData);
        curl_easy_setopt(this->curlSession, CURLOPT_POSTFIELDSIZE, postDataSize);

        // add a Content-Type request header if the post-stream has a content-type set
        if (req->Type.IsValid()) {
            this->stringBuilder.Set("Content-Type: ");
            this->stringBuilder.Append(req->Type.AsCStr());
            requestHeaders = curl_slist_append(requestHeaders, this->stringBuilder.AsCStr());
        }
    }

    // set the http request headers
    if (0 != requestHeaders) {
        curl_easy_setopt(this->curlSession, CURLOPT_HTTPHEADER, requestHeaders);
    }

    // prepare the HTTPResponse and the response-body stream
    Ptr<HTTPProtocol::HTTPResponse> httpResponse = HTTPProtocol::HTTPResponse::Create();
    curl_easy_setopt(this->curlSession, CURLOPT_WRITEDATA, &(httpResponse->Body));

    // perform the request
    CURLcode performResult = curl_easy_perform(this->curlSession);

    // query the http code
    long curlHttpCode = 0;
    curl_easy_getinfo(this->curlSession, CURLINFO_RESPONSE_CODE, &curlHttpCode);
    httpResponse->Status = (IOStatus::Code) curlHttpCode;

    // check for error codes
    if (CURLE_PARTIAL_FILE == performResult) {
        // this seems to happen quite often even though all data has been received,
        // not sure what to do about this, but don't treat it as an error
        Log::Warn("curlURLLoader: CURLE_PARTIAL_FILE received for '%s', httpStatus='%ld'\n", req->Url.AsCStr(), curlHttpCode);
        httpResponse->ErrorDesc = this->curlError;
    }
    else if (0 != performResult) {
        // some other curl error
        Log::Warn("curlURLLoader: curl_easy_peform failed with '%s' for '%s', httpStatus='%ld'\n",
            req->Url.AsCStr(), this->curlError, curlHttpCode);
        httpResponse->ErrorDesc = this->curlError;
    }

    // check if the responseHeaders contained a Content-Type, if yes, set it on the responseBodyStream
    if (this->responseHeaders.Contains(this->contentTypeString)) {
        httpResponse->Type = this->responseHeaders[this->contentTypeString];
    }

    // close the responseBodyStream, and set the result
    httpResponse->ResponseHeaders = this->responseHeaders;
    req->Response = httpResponse;

    // free the previously allocated request headers
    if (0 != requestHeaders) {
        curl_slist_free_all(requestHeaders);
        requestHeaders = 0;
    }
}

} // namespace _priv
} // namespace Oryol
