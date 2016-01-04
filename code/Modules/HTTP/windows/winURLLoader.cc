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
winURLLoader::winURLLoader() :
contentTypeString("Content-Type")
{
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
winURLLoader::doRequest(const Ptr<HTTPProtocol::HTTPRequest>& httpReq) {
    bool result = false;
    if (baseURLLoader::doRequest(httpReq)) {
        this->doRequestInternal(httpReq);
        // transfer result to embedded ioRequest and set to handled
        const Ptr<IOProtocol::Request>& ioReq = httpReq->IoRequest;
        if (ioReq) {
            const Ptr<HTTPProtocol::HTTPResponse>& httpResponse = httpReq->Response;
            ioReq->Status = httpResponse->Status;
            ioReq->Data = std::move(httpResponse->Body);
            ioReq->Type = httpResponse->Type;
            ioReq->ErrorDesc = httpResponse->ErrorDesc;
            ioReq->SetHandled();
        }
        httpReq->SetHandled();
        result = true;
    }
    this->garbageCollectConnections();
    return result;
}

//------------------------------------------------------------------------------
void
winURLLoader::doRequestInternal(const Ptr<HTTPProtocol::HTTPRequest>& req) {
    Log::Info("winURLLoader::doOneRequest() start: %s\n", req->Url.AsCStr());

    // obtain a connection
    HINTERNET hConn = this->obtainConnection(req->Url);
    if (NULL != hConn) {
        WideString method(HTTPMethod::ToWideString(req->Method));
        WideString path(StringConverter::UTF8ToWide(req->Url.PathToEnd()));

        // setup an HTTP request
        HINTERNET hRequest = WinHttpOpenRequest(
            hConn,              // hConnect
            method.AsCStr(),    // pwszVerb
            path.AsCStr(),      // pwszObjectName
            NULL,               // pwszVersion (NULL == HTTP/1.1)
            WINHTTP_NO_REFERER, // pwszReferrer
            WINHTTP_DEFAULT_ACCEPT_TYPES,   // pwszAcceptTypes
            WINHTTP_FLAG_ESCAPE_PERCENT);   // dwFlags
        if (NULL != hRequest) {
            
            // add request headers to the request
            if (!req->RequestHeaders.Empty()) {
                for (const auto& kvp : req->RequestHeaders) {
                    this->stringBuilder.Clear();
                    this->stringBuilder.Append(kvp.Key());
                    this->stringBuilder.Append(": ");
                    this->stringBuilder.Append(kvp.Value());
                    this->stringBuilder.Append("\r\n");
                }

                // check if we need to add a Content-Type field:
                if (req->Type.IsValid()) {
                    this->stringBuilder.Append("Content-Type: ");
                    this->stringBuilder.Append(req->Type.AsCStr());
                    this->stringBuilder.Append("\r\n");
                }

                // remove last CRLF
                this->stringBuilder.PopBack();
                this->stringBuilder.PopBack();
                WideString reqHeaders(StringConverter::UTF8ToWide(this->stringBuilder.GetString()));
                BOOL headerResult = WinHttpAddRequestHeaders(
                    hRequest, 
                    reqHeaders.AsCStr(), 
                    reqHeaders.Length(), 
                    WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE);
                o_assert(headerResult);
            }

            // do we have body-data?
            const uint8* bodyDataPtr = WINHTTP_NO_REQUEST_DATA;
            int32 bodySize = 0;
            if (!req->Body.Empty()) {
                bodyDataPtr = req->Body.Data();
                bodySize = req->Body.Size();
            }

            // create a response object, no matter whether the
            // send fails or not
            Ptr<HTTPProtocol::HTTPResponse> httpResponse = HTTPProtocol::HTTPResponse::Create();
            req->Response = httpResponse;

            // send the request
            BOOL sendResult = WinHttpSendRequest(
                hRequest,                           // hRequest
                WINHTTP_NO_ADDITIONAL_HEADERS,      // pwszHeaders
                0,                                  // dwHeadersLength
                (LPVOID) bodyDataPtr,               // lpOptional
                bodySize,                           // dwOptionalLength
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
                    httpResponse->Status = (IOStatus::Code) dwStatusCode;

                    // query the response headers required data size
                    DWORD dwSize = 0;
                    WinHttpQueryHeaders(hRequest, 
                                        WINHTTP_QUERY_RAW_HEADERS_CRLF,
                                        WINHTTP_HEADER_NAME_BY_INDEX,
                                        NULL,
                                        &dwSize,
                                        WINHTTP_NO_HEADER_INDEX);
                    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                        // and get the response headers
                        LPVOID headerBuffer = Memory::Alloc(dwSize);
                        BOOL headerResult = WinHttpQueryHeaders(
                            hRequest,
                            WINHTTP_QUERY_RAW_HEADERS_CRLF,
                            WINHTTP_HEADER_NAME_BY_INDEX,
                            headerBuffer,
                            &dwSize,
                            WINHTTP_NO_HEADER_INDEX);
                        o_assert(headerResult);
                    
                        // convert from wide and split the header fields
                        this->stringBuilder.Set(StringConverter::WideToUTF8((const wchar_t*) headerBuffer, dwSize / sizeof(wchar_t)));
                        Memory::Free(headerBuffer);
                        Array<String> tokens;
                        this->stringBuilder.Tokenize("\r\n", tokens);
                        Map<String, String> fields;
                        fields.Reserve(tokens.Size());
                        for (const String& str : tokens) {
                            const int32 colonIndex = StringBuilder::FindFirstOf(str.AsCStr(), 0, EndOfString, ":");
                            if (colonIndex != InvalidIndex) {
                                String key(str.AsCStr(), 0, colonIndex);
                                String value(str.AsCStr(), colonIndex+2, EndOfString);
                                fields.Add(key, value);
                            }
                        }
                        httpResponse->ResponseHeaders = fields;
                    }
                    else {
                        Log::Warn("winURLLoader: failed to extract response header fields!\n");
                    }

                    // extract body data
                    DWORD bytesToRead = 0;
                    do {
                        // how much data available?
                        BOOL queryDataResult = WinHttpQueryDataAvailable(hRequest, &bytesToRead);
                        o_assert(queryDataResult);
                        if (dwSize > 0) {
                            uint8* dstPtr = httpResponse->Body.Add(bytesToRead);
                            o_assert(nullptr != dstPtr);
                            DWORD bytesRead = 0;
                            BOOL readDataResult = WinHttpReadData(hRequest, dstPtr, bytesToRead, &bytesRead);
                            o_assert(readDataResult);
                            o_assert(bytesRead == bytesToRead);
                        }
                    }
                    while (bytesToRead > 0);

                    // extract the Content-Type response header field and set on reponse
                    if (httpResponse->ResponseHeaders.Contains(this->contentTypeString)) {
                        ContentType contentType(httpResponse->ResponseHeaders[this->contentTypeString]);
                        httpResponse->Type = contentType;
                    }

                    // @todo: write error desc to httpResponse if something went wrong
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
            port = StringConverter::FromString<int16>(portString);
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
    for (int32 i = this->connections.Size() - 1; i >= 0; i--) {
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

