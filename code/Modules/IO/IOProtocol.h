#pragma once
//-----------------------------------------------------------------------------
/* #version:14#
    machine generated, do not edit!
*/
#include <cstring>
#include "Messaging/Message.h"
#include "Messaging/Protocol.h"
#include "Core/Types.h"
#include "Core/Containers/Buffer.h"
#include "IO/Core/URL.h"
#include "IO/Core/IOStatus.h"
#include "IO/Core/ContentType.h"

namespace Oryol {
class IOProtocol {
public:
    static ProtocolIdType GetProtocolId() {
        return 'IOPT';
    };
    class MessageId {
    public:
        enum {
            RequestId = Protocol::MessageId::NumMessageIds, 
            ReadId,
            WriteId,
            notifyLanesId,
            notifyFileSystemRemovedId,
            notifyFileSystemReplacedId,
            notifyFileSystemAddedId,
            NumMessageIds
        };
        static const char* ToString(MessageIdType c) {
            switch (c) {
                case RequestId: return "RequestId";
                case ReadId: return "ReadId";
                case WriteId: return "WriteId";
                case notifyLanesId: return "notifyLanesId";
                case notifyFileSystemRemovedId: return "notifyFileSystemRemovedId";
                case notifyFileSystemReplacedId: return "notifyFileSystemReplacedId";
                case notifyFileSystemAddedId: return "notifyFileSystemAddedId";
                default: return "InvalidMessageId";
            }
        };
        static MessageIdType FromString(const char* str) {
            if (std::strcmp("RequestId", str) == 0) return RequestId;
            if (std::strcmp("ReadId", str) == 0) return ReadId;
            if (std::strcmp("WriteId", str) == 0) return WriteId;
            if (std::strcmp("notifyLanesId", str) == 0) return notifyLanesId;
            if (std::strcmp("notifyFileSystemRemovedId", str) == 0) return notifyFileSystemRemovedId;
            if (std::strcmp("notifyFileSystemReplacedId", str) == 0) return notifyFileSystemReplacedId;
            if (std::strcmp("notifyFileSystemAddedId", str) == 0) return notifyFileSystemAddedId;
            return InvalidMessageId;
        };
    };
    typedef Ptr<Message> (*CreateCallback)();
    static CreateCallback jumpTable[IOProtocol::MessageId::NumMessageIds];
    class Factory {
    public:
        static Ptr<Message> Create(MessageIdType id);
    };
    class Request : public Message {
        OryolClassDecl(Request);
        OryolTypeDecl(Request,Message);
    public:
        Request() {
            this->msgId = MessageId::RequestId;
            this->CacheReadEnabled = true;
            this->CacheWriteEnabled = true;
            this->StartOffset = 0;
            this->EndOffset = EndOfFile;
            this->Status = IOStatus::InvalidIOStatus;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::RequestId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IOPT') return true;
            else return Message::IsMemberOf(protId);
        };
        URL Url;
        bool CacheReadEnabled;
        bool CacheWriteEnabled;
        int32 StartOffset;
        int32 EndOffset;
        Buffer Data;
        ContentType Type;
        IOStatus::Code Status;
        String ErrorDesc;
    };
    class Read : public Request {
        OryolClassDecl(Read);
        OryolTypeDecl(Read,Request);
    public:
        Read() {
            this->msgId = MessageId::ReadId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::ReadId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IOPT') return true;
            else return Request::IsMemberOf(protId);
        };
    };
    class Write : public Request {
        OryolClassDecl(Write);
        OryolTypeDecl(Write,Request);
    public:
        Write() {
            this->msgId = MessageId::WriteId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::WriteId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IOPT') return true;
            else return Request::IsMemberOf(protId);
        };
    };
    class notifyLanes : public Message {
        OryolClassDecl(notifyLanes);
        OryolTypeDecl(notifyLanes,Message);
    public:
        notifyLanes() {
            this->msgId = MessageId::notifyLanesId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::notifyLanesId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IOPT') return true;
            else return Message::IsMemberOf(protId);
        };
        StringAtom Scheme;
    };
    class notifyFileSystemRemoved : public notifyLanes {
        OryolClassDecl(notifyFileSystemRemoved);
        OryolTypeDecl(notifyFileSystemRemoved,notifyLanes);
    public:
        notifyFileSystemRemoved() {
            this->msgId = MessageId::notifyFileSystemRemovedId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::notifyFileSystemRemovedId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IOPT') return true;
            else return notifyLanes::IsMemberOf(protId);
        };
    };
    class notifyFileSystemReplaced : public notifyLanes {
        OryolClassDecl(notifyFileSystemReplaced);
        OryolTypeDecl(notifyFileSystemReplaced,notifyLanes);
    public:
        notifyFileSystemReplaced() {
            this->msgId = MessageId::notifyFileSystemReplacedId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::notifyFileSystemReplacedId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IOPT') return true;
            else return notifyLanes::IsMemberOf(protId);
        };
    };
    class notifyFileSystemAdded : public notifyLanes {
        OryolClassDecl(notifyFileSystemAdded);
        OryolTypeDecl(notifyFileSystemAdded,notifyLanes);
    public:
        notifyFileSystemAdded() {
            this->msgId = MessageId::notifyFileSystemAddedId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::notifyFileSystemAddedId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IOPT') return true;
            else return notifyLanes::IsMemberOf(protId);
        };
    };
};
}
