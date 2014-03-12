#pragma once
//-----------------------------------------------------------------------------
/*
    machine generated, do not edit!
*/
#include <cstring>
#include "Messaging/Message.h"
#include "Messaging/Serializer.h"
#include "Messaging/Protocol.h"
#include "Render/DisplayAttrs.h"

namespace Oryol {
namespace Render {
class RenderProtocol {
public:
    static Messaging::ProtocolIdType GetProtocolId() {
        return 'RRPT';
    };
    class MessageId {
    public:
        enum {
            DisplaySetupId = Messaging::Protocol::MessageId::NumMessageIds, 
            DisplayDiscardedId,
            DisplayModifiedId,
            NumMessageIds
        };
        static const char* ToString(Messaging::MessageIdType c) {
            switch (c) {
                case DisplaySetupId: return "DisplaySetupId";
                case DisplayDiscardedId: return "DisplayDiscardedId";
                case DisplayModifiedId: return "DisplayModifiedId";
                default: return "InvalidMessageId";
            }
        };
        static Messaging::MessageIdType FromString(const char* str) {
            if (std::strcmp("DisplaySetupId", str) == 0) return DisplaySetupId;
            if (std::strcmp("DisplayDiscardedId", str) == 0) return DisplayDiscardedId;
            if (std::strcmp("DisplayModifiedId", str) == 0) return DisplayModifiedId;
            return Messaging::InvalidMessageId;
        };
    };
    typedef Messaging::Message* (*CreateCallback)();
    static CreateCallback jumpTable[RenderProtocol::MessageId::NumMessageIds];
    class Factory {
    public:
        static Messaging::Message* Create(Messaging::MessageIdType id);
    };
    class DisplaySetup : public Messaging::Message {
        OryolClassPoolAllocDecl(DisplaySetup);
    public:
        DisplaySetup() {
            this->msgId = MessageId::DisplaySetupId;
        };
        static Messaging::Message* FactoryCreate() {
            return (Messaging::Message*) Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::DisplaySetupId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'RRPT') return true;
            else return Messaging::Message::IsMemberOf(protId);
        };
private:
    };
    class DisplayDiscarded : public Messaging::Message {
        OryolClassPoolAllocDecl(DisplayDiscarded);
    public:
        DisplayDiscarded() {
            this->msgId = MessageId::DisplayDiscardedId;
        };
        static Messaging::Message* FactoryCreate() {
            return (Messaging::Message*) Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::DisplayDiscardedId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'RRPT') return true;
            else return Messaging::Message::IsMemberOf(protId);
        };
private:
    };
    class DisplayModified : public Messaging::Message {
        OryolClassPoolAllocDecl(DisplayModified);
    public:
        DisplayModified() {
            this->msgId = MessageId::DisplayModifiedId;
        };
        static Messaging::Message* FactoryCreate() {
            return (Messaging::Message*) Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::DisplayModifiedId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'RRPT') return true;
            else return Messaging::Message::IsMemberOf(protId);
        };
private:
    };
};
}
}
