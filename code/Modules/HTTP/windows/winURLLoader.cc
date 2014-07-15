//------------------------------------------------------------------------------
//  winURLLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "winURLLoader.h"
#include "Core/String/StringConverter.h"
#include "IO/Stream/MemoryStream.h"
#define VC_EXTRALEAN (1)
#define WIN32_LEAN_AND_MEAN (1)
#include <Windows.h>
#include <WinHttp.h>

namespace Oryol {
namespace HTTP {

using namespace Core;
using namespace IO;

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
void
winURLLoader::doWork() {
    while (!this->requestQueue.Empty()) {
        Ptr<HTTPProtocol::HTTPRequest> req = this->requestQueue.Dequeue();
        this->doOneRequest(req);
        req->SetHandled();
    }
    this->garbageCollectConnections();
}

//------------------------------------------------------------------------------
void
winURLLoader::doOneRequest(const Ptr<HTTPProtocol::HTTPRequest>& req) {
    // obtain a connection
    HINTERNET hConn = this->obtainConnection(req->GetURL());
    if (NULL != hConn) {
        WideString method(HTTPMethod::ToWideString(req->GetMethod()));
        WideString path(StringConverter::UTF8ToWide(req->GetURL().PathToEnd()));

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
            if (!req->GetRequestHeaders().Empty()) {
                for (const auto& kvp : req->GetRequestHeaders()) {
                    this->stringBuilder.Clear();
                    this->stringBuilder.Append(kvp.Key());
                    this->stringBuilder.Append(": ");
                    this->stringBuilder.Append(kvp.Value());
                    this->stringBuilder.Append("\r\n");
                }

                // check if we need to add a Content-Type field:
                if (req->GetBody().isValid() && req->GetBody()->GetContentType().IsValid()) {
                    this->stringBuilder.Append("Content-Type: ");
                    this->stringBuilder.Append(req->GetBody()->GetContentType().AsCStr());
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
            if (req->GetBody().isValid()) {
                const Ptr<Stream>& bodyStream = req->GetBody();
                bodySize = bodyStream->Size();
                o_assert(bodySize > 0);
                bodyStream->Open(OpenMode::ReadOnly);
                bodyDataPtr = bodyStream->MapRead(nullptr);
            }

            // create a response object, no matter whether the
            // send fails or not
            Ptr<HTTPProtocol::HTTPResponse> httpResponse = HTTPProtocol::HTTPResponse::Create();
            req->SetResponse(httpResponse);

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
                    httpResponse->SetStatus((IOStatus::Code) dwStatusCode);

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
                        Array<String> tokens;
                        this->stringBuilder.Tokenize("\r\n", tokens);
                        Map<String, String> fields;
                        fields.Reserve(tokens.Size());
                        for (const String& str : tokens) {
                            const int32 colonIndex = StringBuilder::FindFirstOf(str.AsCStr(), 0, EndOfString, ":");
                            if (colonIndex != InvalidIndex) {
                                String key(str.AsCStr(), 0, colonIndex);
                                String value(str.AsCStr(), colonIndex+2, EndOfString);
                                fields.Insert(key, value);
                            }
                        }
                        httpResponse->SetResponseHeaders(fields);
                    }
                    else {
                        Log::Warn("winURLLoader: failed to extract response header fields!\n");
                    }

                    // extract body data
                    Ptr<MemoryStream> responseBodyStream = MemoryStream::Create();
                    responseBodyStream->Open(OpenMode::WriteOnly);
                    DWORD bytesToRead = 0;
                    do {
                        // how much data available?
                        BOOL queryDataResult = WinHttpQueryDataAvailable(hRequest, &bytesToRead);
                        o_assert(queryDataResult);
                        if (dwSize > 0) {
                            uint8* dstPtr = responseBodyStream->MapWrite(bytesToRead);
                            o_assert(nullptr != dstPtr);
                            DWORD bytesRead = 0;
                            BOOL readDataResult = WinHttpReadData(hRequest, dstPtr, bytesToRead, &bytesRead);
                            o_assert(readDataResult);
                            o_assert(bytesRead == bytesToRead);
                            responseBodyStream->UnmapWrite();
                        }
                    }
                    while (bytesToRead > 0);
                    responseBodyStream->Close();

                    // extract the Content-Type response header field and set on responseBodyStream
                    if (httpResponse->GetResponseHeaders().Contains(this->contentTypeString)) {
                        ContentType contentType(httpResponse->GetResponseHeaders()[this->contentTypeString]);
                        responseBodyStream->SetContentType(contentType);
                    }

                    // ...and finally set the responseBodyStream on the httpResponse
                    httpResponse->SetBody(responseBodyStream);

                    // @todo: write error desc to httpResponse if something went wrong
                }
                else {
                    Log::Warn("winURLLoader: WinHttpReceiveResponse() failed for '%s'!\n", req->GetURL().AsCStr());
                }
            }
            else {
                /// @todo: better error reporting!
                Log::Warn("winURLLoader: WinHttpSendRequest() failed for '%s'\n", req->GetURL().AsCStr());
            }

            // unmap the body data if necessary
            if (req->GetBody().isValid()) {
                req->GetBody()->Close();
            }

            // close the request object
            WinHttpCloseHandle(hRequest);
        }
        else {
            Log::Warn("winURLLoader: WinHttpOpenRequest() failed for '%s'\n", req->GetURL().AsCStr());
        }
    }
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
            this->connections.Insert(hostAndPort, con);
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

} // namespace HTTP
} // namespace Oryol