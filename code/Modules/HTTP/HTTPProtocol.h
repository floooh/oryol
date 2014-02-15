#pragma once
//-----------------------------------------------------------------------------
/*
    machine generated, do not edit!
*/
#include <cstring>
#include "Messaging/Message.h"
#include "Messaging/Serializer.h"
#include "Messaging/Protocol.h"
#include "IO/URL.h"
#include "HTTP/HTTPResponse.h"
#include "HTTP/HTTPMethod.h"
#include "Core/Containers/Map.h"
#include "Core/String/String.h"
#include "IO/Stream.h"

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
            HTTPRequestId = Messaging::Protocol::MessageId::NumMessageIds, 
            NumMessageIds
        };
        static const char* ToString(Messaging::MessageIdType c) {
            switch (c) {
                case HTTPRequestId: return "HTTPRequestId";
                default: return "InvalidMessageId";
            }
        };
        static Messaging::MessageIdType FromString(const char* str) {
            if (std::strcmp("HTTPRequestId", str) == 0) return HTTPRequestId;
            return Messaging::InvalidMessageId;
        };
    };
    typedef Messaging::Message* (*CreateCallback)();
    static CreateCallback jumpTable[HTTPProtocol::MessageId::NumMessageIds];
    class Factory {
    public:
        static Messaging::Message* Create(Messaging::MessageIdType id);
    };
    class HTTPRequest : public Messaging::Message {
        OryolClassPoolAllocDecl(HTTPRequest);
    public:
        HTTPRequest() {
            this->msgId = MessageId::HTTPRequestId;
        };
        static Messaging::Message* FactoryCreate() {
            return (Messaging::Message*) Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::HTTPRequestId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'HTPR') return true;
            else return Messaging::Message::IsMemberOf(protId);
        };
        void SetMethod(const HTTP::HTTPMethod& val) {
            this->method = val;
        };
        const HTTP::HTTPMethod& GetMethod() const {
            return this->method;
        };
        void SetURL(const IO::URL& val) {
            this->url = val;
        };
        const IO::URL& GetURL() const {
            return this->url;
        };
        void SetFields(const Core::Map<Core::String,Core::String>& val) {
            this->fields = val;
        };
        const Core::Map<Core::String,Core::String>& GetFields() const {
            return this->fields;
        };
        void SetBody(const Core::Ptr<IO::Stream>& val) {
            this->body = val;
        };
        const Core::Ptr<IO::Stream>& GetBody() const {
            return this->body;
        };
        void SetResponse(const Core::Ptr<HTTP::HTTPResponse>& val) {
            this->response = val;
        };
        const Core::Ptr<HTTP::HTTPResponse>& GetResponse() const {
            return this->response;
        };
private:
        HTTP::HTTPMethod method;
        IO::URL url;
        Core::Map<Core::String,Core::String> fields;
        Core::Ptr<IO::Stream> body;
        Core::Ptr<HTTP::HTTPResponse> response;
    };
};
}
}
