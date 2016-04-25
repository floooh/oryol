//------------------------------------------------------------------------------
//  winURLLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "winURLLoader.h"
#include "Core/String/StringConverter.h"
#define VC_EXTRALEAN (1)
#define WIN32_LEAN_AND_MEAN (1)
#include <Windows.h>
#include <WinHttp.h>

namespace Oryol {
namespace _priv {

const std::chrono::seconds winURLLoader::connectionMaxAge{10};

//------------------------------------------------------------------------------
winURLLoader::winURLLoader() {
    this->hSession = WinHttpOpen(NULL,      // pwszUserAgent
                                 WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,     // dwAccessType
                                 WINHTTP_NO_PROXY_NAME, // pwszProxyName
                                 WINHTTP_NO_PROXY_BYPASS,   // pwszProxyByPass
                                 0);        // dwFlags
    o_assert(NULL != this->hSession);
}

//------------------------------------------------------------------------------
winURLLoader::~winURLLoader() {
    o_assert(NULL != this->hSession);
    // close remaining connections
    for (const auto& kvp : this->connections) {
        WinHttpCloseHandle(kvp.Value().hConnection);
    }
    this->connections.Clear();
    WinHttpCloseHandle(this->hSession);
    this->hSession = NULL;
}

//------------------------------------------------------------------------------
bool
winURLLoader::doRequest(const Ptr<IORead>& req) {
    bool result = false;
    if (baseURLLoader::doRequest(req)) {
        this->doRequestInternal(req);
        req->Handled = true;
    }
    this->garbageCollectConnections();
    return result;
}

//------------------------------------------------------------------------------
void
winURLLoader::doRequestInternal(const Ptr<IORead>& req) {
    Log::Info("winURLLoader::doOneRequest() start: %s\n", req->Url.AsCStr());

    // obtain a connection
    HINTERNET hConn = this->obtainConnection(req->Url);
    if (NULL != hConn) {
        WideString path(StringConverter::UTF8ToWide(req->Url.PathToEnd()));

        // setup an HTTP request
        HINTERNET hRequest = WinHttpOpenRequest(
            hConn,              // hConnect
            L"GET",             // pwszVerb
            path.AsCStr(),      // pwszObjectName
            NULL,               // pwszVersion (NULL == HTTP/1.1)
            WINHTTP_NO_REFERER, // pwszReferrer
            WINHTTP_DEFAULT_ACCEPT_TYPES,   // pwszAcceptTypes
            WINHTTP_FLAG_ESCAPE_PERCENT);   // dwFlags
        if (NULL != hRequest) {
            
            // add request headers to the request
            const wchar_t* reqHeaders = 
                L"User-Agent: Mozilla/5.0\r\n"
                L"Connection: keep-alive\r\n"
                L"Accept-Encoding: gzip, deflate";
            BOOL headerResult = WinHttpAddRequestHeaders(
                hRequest, 
                reqHeaders, -1L, 
                WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE);
            o_assert(headerResult);

            // send the request
            BOOL sendResult = WinHttpSendRequest(
                hRequest,                           // hRequest
                WINHTTP_NO_ADDITIONAL_HEADERS,      // pwszHeaders
                0,                                  // dwHeadersLength
                WINHTTP_NO_REQUEST_DATA,            // lpOptional
                0,                                  // dwOptionalLength
                0,                                  // dwTotoalLength
                0);                                 // dwContext
            if (sendResult) {
                // receive and process response
                BOOL recvResult = WinHttpReceiveResponse(hRequest, NULL);
                if (recvResult) {
                    // query the HTTP status code
                    DWORD dwStatusCode = 0;
                    DWORD dwTemp = sizeof(dwStatusCode);
                    WinHttpQueryHeaders(
                        hRequest, 
                        WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,
                        NULL,
                        &dwStatusCode,
                        &dwTemp,
                        NULL);
                    req->Status = (IOStatus::Code) dwStatusCode;

                    // extract body data
                    DWORD bytesToRead = 0;
                    do {
                        // how much data available?
                        BOOL queryDataResult = WinHttpQueryDataAvailable(hRequest, &bytesToRead);
                        o_assert(queryDataResult);
                        if (bytesToRead > 0) {
                            uint8_t* dstPtr = req->Data.Add(bytesToRead);
                            o_assert(nullptr != dstPtr);
                            DWORD bytesRead = 0;
                            BOOL readDataResult = WinHttpReadData(hRequest, dstPtr, bytesToRead, &bytesRead);
                            o_assert(readDataResult);
                            o_assert(bytesRead == bytesToRead);
                        }
                    }
                    while (bytesToRead > 0);

                    // @todo: write error desc to msg if something went wrong
                }
                else {
                    Log::Warn("winURLLoader: WinHttpReceiveResponse() failed for '%s'!\n", req->Url.AsCStr());
                }
            }
            else {
                /// @todo: better error reporting!
                Log::Warn("winURLLoader: WinHttpSendRequest() failed for '%s'\n", req->Url.AsCStr());
            }

            // close the request object
            WinHttpCloseHandle(hRequest);
        }
        else {
            Log::Warn("winURLLoader: WinHttpOpenRequest() failed for '%s'\n", req->Url.AsCStr());
        }
    }
    Log::Info("winURLLoader::doOneRequest() end: %s\n", req->Url.AsCStr());
}

//------------------------------------------------------------------------------
HINTERNET
winURLLoader::obtainConnection(const URL& url) {
    o_assert(NULL != this->hSession);
    const String hostAndPort = url.HostAndPort();
    if (this->connections.Contains(hostAndPort)) {
        // connection was already established, refresh the timestamp
        // and return the connection
        connection& con = this->connections[hostAndPort];
        con.timeStamp = std::chrono::steady_clock::now();
        return con.hConnection;
    }
    else {
        // new connection
        connection con;
        const WideString host = StringConverter::UTF8ToWide(url.Host());
        const String portString = url.Port();
        INTERNET_PORT port = INTERNET_DEFAULT_HTTP_PORT;
        if (!portString.Empty()) {
            port = StringConverter::FromString<int16_t>(portString);
        }
        con.hConnection = WinHttpConnect(this->hSession,        // hSession
                                          host.AsCStr(),        // pswzServerName
                                          port,                 // nServerPort
                                          0);                   // dwReserved
        if (NULL != con.hConnection) {
            con.timeStamp = std::chrono::steady_clock::now();
            this->connections.Add(hostAndPort, con);
            return con.hConnection;        
        }   
        else {
            Log::Warn("winURLLoader: failed to connect to '%s'\n", hostAndPort.AsCStr());
            return NULL;
        } 
    }
}

//------------------------------------------------------------------------------
void
winURLLoader::garbageCollectConnections() {
    std::chrono::steady_clock::time_point curTime = std::chrono::steady_clock::now();
    for (int i = this->connections.Size() - 1; i >= 0; i--) {
        const String key = this->connections.KeyAtIndex(i);
        const connection con = this->connections.ValueAtIndex(i);
        std::chrono::seconds age = std::chrono::duration_cast<std::chrono::seconds>(curTime - con.timeStamp); 
        if (age > connectionMaxAge) {
            o_assert(NULL != con.hConnection);
            WinHttpCloseHandle(con.hConnection);
            this->connections.EraseIndex(i);
        }
    }
}

} // namespace _priv
} // namespace Oryol

