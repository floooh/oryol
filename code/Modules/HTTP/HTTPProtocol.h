#pragma once
//-----------------------------------------------------------------------------
/* #version:10#
    machine generated, do not edit!
*/
#include <cstring>
#include "Messaging/Message.h"
#include "Messaging/Serializer.h"
#include "Messaging/Protocol.h"
#include "IO/Core/URL.h"
#include "HTTP/HTTPMethod.h"
#include "Core/Containers/Map.h"
#include "Core/String/String.h"
#include "IO/Stream/Stream.h"
#include "IO/Core/IOStatus.h"
#include "IO/IOProtocol.h"

namespace Oryol {
class HTTPProtocol {
public:
    static ProtocolIdType GetProtocolId() {
        return 'HTPR';
    };
    class MessageId {
    public:
        enum {
            HTTPResponseId = Protocol::MessageId::NumMessageIds, 
            HTTPRequestId,
            NumMessageIds
        };
        static const char* ToString(MessageIdType c) {
            switch (c) {
                case HTTPResponseId: return "HTTPResponseId";
                case HTTPRequestId: return "HTTPRequestId";
                default: return "InvalidMessageId";
            }
        };
        static MessageIdType FromString(const char* str) {
            if (std::strcmp("HTTPResponseId", str) == 0) return HTTPResponseId;
            if (std::strcmp("HTTPRequestId", str) == 0) return HTTPRequestId;
            return InvalidMessageId;
        };
    };
    typedef Ptr<Message> (*CreateCallback)();
    static CreateCallback jumpTable[HTTPProtocol::MessageId::NumMessageIds];
    class Factory {
    public:
        static Ptr<Message> Create(MessageIdType id);
    };
    class HTTPResponse : public Message {
        OryolClassDecl(HTTPResponse);
        OryolTypeDecl(HTTPResponse,Message);
    public:
        HTTPResponse() {
            this->msgId = MessageId::HTTPResponseId;
            this->status = IOStatus::InvalidIOStatus;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::HTTPResponseId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const {
            if (protId == 'HTPR') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetStatus(const IOStatus::Code& val) {
            this->status = val;
        };
        const IOStatus::Code& GetStatus() const {
            return this->status;
        };
        void SetResponseHeaders(const Map<String,String>& val) {
            this->responseheaders = val;
        };
        const Map<String,String>& GetResponseHeaders() const {
            return this->responseheaders;
        };
        void SetBody(const Ptr<Stream>& val) {
            this->body = val;
        };
        const Ptr<Stream>& GetBody() const {
            return this->body;
        };
        void SetErrorDesc(const String& val) {
            this->errordesc = val;
        };
        const String& GetErrorDesc() const {
            return this->errordesc;
        };
private:
        IOStatus::Code status;
        Map<String,String> responseheaders;
        Ptr<Stream> body;
        String errordesc;
    };
    class HTTPRequest : public Message {
        OryolClassDecl(HTTPRequest);
        OryolTypeDecl(HTTPRequest,Message);
    public:
        HTTPRequest() {
            this->msgId = MessageId::HTTPRequestId;
            this->method = HTTPMethod::Get;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::HTTPRequestId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const {
            if (protId == 'HTPR') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetMethod(const HTTPMethod::Code& val) {
            this->method = val;
        };
        const HTTPMethod::Code& GetMethod() const {
            return this->method;
        };
        void SetURL(const URL& val) {
            this->url = val;
        };
        const URL& GetURL() const {
            return this->url;
        };
        void SetRequestHeaders(const Map<String,String>& val) {
            this->requestheaders = val;
        };
        const Map<String,String>& GetRequestHeaders() const {
            return this->requestheaders;
        };
        void SetBody(const Ptr<Stream>& val) {
            this->body = val;
        };
        const Ptr<Stream>& GetBody() const {
            return this->body;
        };
        void SetIoRequest(const Ptr<IOProtocol::Request>& val) {
            this->iorequest = val;
        };
        const Ptr<IOProtocol::Request>& GetIoRequest() const {
            return this->iorequest;
        };
        void SetResponse(const Ptr<HTTPProtocol::HTTPResponse>& val) {
            this->response = val;
        };
        const Ptr<HTTPProtocol::HTTPResponse>& GetResponse() const {
            return this->response;
        };
private:
        HTTPMethod::Code method;
        URL url;
        Map<String,String> requestheaders;
        Ptr<Stream> body;
        Ptr<IOProtocol::Request> iorequest;
        Ptr<HTTPProtocol::HTTPResponse> response;
    };
};
}
