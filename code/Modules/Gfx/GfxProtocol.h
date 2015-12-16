#pragma once
//-----------------------------------------------------------------------------
/* #version:12#
    machine generated, do not edit!
*/
#include <cstring>
#include "Messaging/Message.h"
#include "Messaging/Protocol.h"

namespace Oryol {
class GfxProtocol {
public:
    static ProtocolIdType GetProtocolId() {
        return 'GXPT';
    };
    class MessageId {
    public:
        enum {
            DisplaySetupId = Protocol::MessageId::NumMessageIds, 
            DisplayDiscardedId,
            DisplayModifiedId,
            NumMessageIds
        };
        static const char* ToString(MessageIdType c) {
            switch (c) {
                case DisplaySetupId: return "DisplaySetupId";
                case DisplayDiscardedId: return "DisplayDiscardedId";
                case DisplayModifiedId: return "DisplayModifiedId";
                default: return "InvalidMessageId";
            }
        };
        static MessageIdType FromString(const char* str) {
            if (std::strcmp("DisplaySetupId", str) == 0) return DisplaySetupId;
            if (std::strcmp("DisplayDiscardedId", str) == 0) return DisplayDiscardedId;
            if (std::strcmp("DisplayModifiedId", str) == 0) return DisplayModifiedId;
            return InvalidMessageId;
        };
    };
    typedef Ptr<Message> (*CreateCallback)();
    static CreateCallback jumpTable[GfxProtocol::MessageId::NumMessageIds];
    class Factory {
    public:
        static Ptr<Message> Create(MessageIdType id);
    };
    class DisplaySetup : public Message {
        OryolClassDecl(DisplaySetup);
        OryolTypeDecl(DisplaySetup,Message);
    public:
        DisplaySetup() {
            this->msgId = MessageId::DisplaySetupId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::DisplaySetupId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'GXPT') return true;
            else return Message::IsMemberOf(protId);
        };
private:
    };
    class DisplayDiscarded : public Message {
        OryolClassDecl(DisplayDiscarded);
        OryolTypeDecl(DisplayDiscarded,Message);
    public:
        DisplayDiscarded() {
            this->msgId = MessageId::DisplayDiscardedId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::DisplayDiscardedId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'GXPT') return true;
            else return Message::IsMemberOf(protId);
        };
private:
    };
    class DisplayModified : public Message {
        OryolClassDecl(DisplayModified);
        OryolTypeDecl(DisplayModified,Message);
    public:
        DisplayModified() {
            this->msgId = MessageId::DisplayModifiedId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::DisplayModifiedId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'GXPT') return true;
            else return Message::IsMemberOf(protId);
        };
private:
    };
};
}
