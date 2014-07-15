#pragma once
//-----------------------------------------------------------------------------
/* #version:2#
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

namespace Oryol {
namespace HTTP {
class HTTPProtocol {
public:
    static Messaging::ProtocolIdType GetProtocolId() {
        return 'HTPR';
    };
    class MessageId {
    public:
        enum {
            HTTPResponseId = Messaging::Protocol::MessageId::NumMessageIds, 
            HTTPRequestId,
            NumMessageIds
        };
        static const char* ToString(Messaging::MessageIdType c) {
            switch (c) {
                case HTTPResponseId: return "HTTPResponseId";
                case HTTPRequestId: return "HTTPRequestId";
                default: return "InvalidMessageId";
            }
        };
        static Messaging::MessageIdType FromString(const char* str) {
            if (std::strcmp("HTTPResponseId", str) == 0) return HTTPResponseId;
            if (std::strcmp("HTTPRequestId", str) == 0) return HTTPRequestId;
            return Messaging::InvalidMessageId;
        };
    };
    typedef Core::Ptr<Messaging::Message> (*CreateCallback)();
    static CreateCallback jumpTable[HTTPProtocol::MessageId::NumMessageIds];
    class Factory {
    public:
        static Core::Ptr<Messaging::Message> Create(Messaging::MessageIdType id);
    };
    class HTTPResponse : public Messaging::Message {
        OryolClassPoolAllocDecl(HTTPResponse);
    public:
        HTTPResponse() {
            this->msgId = MessageId::HTTPResponseId;
            this->status = IO::IOStatus::InvalidIOStatus;
        };
        static Core::Ptr<Messaging::Message> FactoryCreate() {
            return Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::HTTPResponseId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'HTPR') return true;
            else return Messaging::Message::IsMemberOf(protId);
        };
        void SetStatus(const IO::IOStatus::Code& val) {
            this->status = val;
        };
        const IO::IOStatus::Code& GetStatus() const {
            return this->status;
        };
        void SetResponseHeaders(const Core::Map<Core::String,Core::String>& val) {
            this->responseheaders = val;
        };
        const Core::Map<Core::String,Core::String>& GetResponseHeaders() const {
            return this->responseheaders;
        };
        void SetBody(const Core::Ptr<IO::Stream>& val) {
            this->body = val;
        };
        const Core::Ptr<IO::Stream>& GetBody() const {
            return this->body;
        };
        void SetErrorDesc(const Core::String& val) {
            this->errordesc = val;
        };
        const Core::String& GetErrorDesc() const {
            return this->errordesc;
        };
private:
        IO::IOStatus::Code status;
        Core::Map<Core::String,Core::String> responseheaders;
        Core::Ptr<IO::Stream> body;
        Core::String errordesc;
    };
    class HTTPRequest : public Messaging::Message {
        OryolClassPoolAllocDecl(HTTPRequest);
    public:
        HTTPRequest() {
            this->msgId = MessageId::HTTPRequestId;
            this->method = HTTP::HTTPMethod::Get;
        };
        static Core::Ptr<Messaging::Message> FactoryCreate() {
            return Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::HTTPRequestId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'HTPR') return true;
            else return Messaging::Message::IsMemberOf(protId);
        };
        void SetMethod(const HTTP::HTTPMethod::Code& val) {
            this->method = val;
        };
        const HTTP::HTTPMethod::Code& GetMethod() const {
            return this->method;
        };
        void SetURL(const IO::URL& val) {
            this->url = val;
        };
        const IO::URL& GetURL() const {
            return this->url;
        };
        void SetRequestHeaders(const Core::Map<Core::String,Core::String>& val) {
            this->requestheaders = val;
        };
        const Core::Map<Core::String,Core::String>& GetRequestHeaders() const {
            return this->requestheaders;
        };
        void SetBody(const Core::Ptr<IO::Stream>& val) {
            this->body = val;
        };
        const Core::Ptr<IO::Stream>& GetBody() const {
            return this->body;
        };
        void SetResponse(const Core::Ptr<HTTPProtocol::HTTPResponse>& val) {
            this->response = val;
        };
        const Core::Ptr<HTTPProtocol::HTTPResponse>& GetResponse() const {
            return this->response;
        };
private:
        HTTP::HTTPMethod::Code method;
        IO::URL url;
        Core::Map<Core::String,Core::String> requestheaders;
        Core::Ptr<IO::Stream> body;
        Core::Ptr<HTTPProtocol::HTTPResponse> response;
    };
};
}
}
