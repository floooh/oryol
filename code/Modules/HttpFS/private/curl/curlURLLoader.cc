//------------------------------------------------------------------------------
//  curlURLLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "curlURLLoader.h"
#include "Core/String/StringConverter.h"
#include "Core/Containers/Buffer.h"
#include "curl/curl.h"
#include <mutex>

#if LIBCURL_VERSION_NUM != 0x072400
#error "Not using the right curl version, header search path fuckup?"
#endif

namespace Oryol {
namespace _priv {

static bool curlInitCalled = false;
static std::mutex curlInitMutex;

//------------------------------------------------------------------------------
curlURLLoader::curlURLLoader() :
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
    const int curlErrorBufferSize = CURL_ERROR_SIZE * 4;
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
    int bytesToWrite = (int) (size * nmemb);
    if (bytesToWrite > 0) {
        Buffer* buf = (Buffer*) userData;
        buf->Add((const uint8_t*)ptr, bytesToWrite);
        return bytesToWrite;
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
bool
curlURLLoader::doRequest(const Ptr<IORead>& req) {
    if (baseURLLoader::doRequest(req)) {
        this->doRequestInternal(req);
        req->Handled = true;
        return true;
    }
    else {
        // request was cancelled
        return false;
    }
}

//------------------------------------------------------------------------------
void
curlURLLoader::doRequestInternal(const Ptr<IORead>& req) {
    o_assert(0 != this->curlSession);
    o_assert(0 != this->curlError);

    // set URL in curl
    const URL& url = req->Url;
    o_assert(url.Scheme() == "http");
    curl_easy_setopt(this->curlSession, CURLOPT_URL, url.AsCStr());
    if (url.HasPort()) {
        uint16_t port = StringConverter::FromString<uint16_t>(url.Port());
        curl_easy_setopt(this->curlSession, CURLOPT_PORT, port);
    }
    curl_easy_setopt(this->curlSession, CURLOPT_HTTPGET, 1);

    // add standard request headers:
    //  User-Agent: need a 'standard' user-agent, otherwise some HTTP servers
    //              won't accept Connection: keep-alive
    //  Connection: keep-alive, don't open/close the connection all the time
    //  Accept-Encoding:    gzip, deflate
    //
    struct curl_slist* requestHeaders = 0;
    requestHeaders = curl_slist_append(requestHeaders, "User-Agent: Mozilla/5.0");
    requestHeaders = curl_slist_append(requestHeaders, "Connection: keep-alive");
    requestHeaders = curl_slist_append(requestHeaders, "Accept-Encoding: gzip, deflate");
    curl_easy_setopt(this->curlSession, CURLOPT_HTTPHEADER, requestHeaders);

    // prepare the HTTPResponse and the response-body stream
    curl_easy_setopt(this->curlSession, CURLOPT_WRITEDATA, &(req->Data));

    // perform the request
    CURLcode performResult = curl_easy_perform(this->curlSession);

    // query the http code
    long curlHttpCode = 0;
    curl_easy_getinfo(this->curlSession, CURLINFO_RESPONSE_CODE, &curlHttpCode);
    req->Status = (IOStatus::Code) curlHttpCode;

    // check for error codes
    if (CURLE_PARTIAL_FILE == performResult) {
        // this seems to happen quite often even though all data has been received,
        // not sure what to do about this, but don't treat it as an error
        Log::Warn("curlURLLoader: CURLE_PARTIAL_FILE received for '%s', httpStatus='%ld'\n", req->Url.AsCStr(), curlHttpCode);
        req->ErrorDesc = this->curlError;
    }
    else if (0 != performResult) {
        // some other curl error
        Log::Warn("curlURLLoader: curl_easy_peform failed with '%s' for '%s', httpStatus='%ld'\n",
            req->Url.AsCStr(), this->curlError, curlHttpCode);
        req->ErrorDesc = this->curlError;
    }

    // free the previously allocated request headers
    if (0 != requestHeaders) {
        curl_slist_free_all(requestHeaders);
        requestHeaders = 0;
    }
}

} // namespace _priv
} // namespace Oryol
