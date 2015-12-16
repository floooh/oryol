#pragma once
//-----------------------------------------------------------------------------
/* #version:14#
    machine generated, do not edit!
*/
#include <cstring>
#include "Messaging/Message.h"
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
            this->Status = IOStatus::InvalidIOStatus;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::HTTPResponseId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'HTPR') return true;
            else return Message::IsMemberOf(protId);
        };
        IOStatus::Code Status;
        Map<String,String> ResponseHeaders;
        Ptr<Stream> Body;
        String ErrorDesc;
    };
    class HTTPRequest : public Message {
        OryolClassDecl(HTTPRequest);
        OryolTypeDecl(HTTPRequest,Message);
    public:
        HTTPRequest() {
            this->msgId = MessageId::HTTPRequestId;
            this->Method = HTTPMethod::Get;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::HTTPRequestId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'HTPR') return true;
            else return Message::IsMemberOf(protId);
        };
        HTTPMethod::Code Method;
        URL Url;
        Map<String,String> RequestHeaders;
        Ptr<Stream> Body;
        Ptr<IOProtocol::Request> IoRequest;
        Ptr<HTTPProtocol::HTTPResponse> Response;
    };
};
}
