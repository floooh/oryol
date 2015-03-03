#pragma once
//-----------------------------------------------------------------------------
/* #version:8#
    machine generated, do not edit!
*/
#include <cstring>
#include "Messaging/Message.h"
#include "Messaging/Serializer.h"
#include "Messaging/Protocol.h"
#include "Core/Ptr.h"
#include "IO/Core/URL.h"
#include "IO/Core/IOStatus.h"
#include "IO/Stream/MemoryStream.h"

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
            notifyLanesId,
            notifyFileSystemRemovedId,
            notifyFileSystemReplacedId,
            notifyFileSystemAddedId,
            NumMessageIds
        };
        static const char* ToString(MessageIdType c) {
            switch (c) {
                case RequestId: return "RequestId";
                case notifyLanesId: return "notifyLanesId";
                case notifyFileSystemRemovedId: return "notifyFileSystemRemovedId";
                case notifyFileSystemReplacedId: return "notifyFileSystemReplacedId";
                case notifyFileSystemAddedId: return "notifyFileSystemAddedId";
                default: return "InvalidMessageId";
            }
        };
        static MessageIdType FromString(const char* str) {
            if (std::strcmp("RequestId", str) == 0) return RequestId;
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
        OryolClassPoolAllocDecl(Request);
    public:
        Request() {
            this->msgId = MessageId::RequestId;
            this->lane = 0;
            this->cachereadenabled = true;
            this->cachewriteenabled = true;
            this->startoffset = 0;
            this->endoffset = 0;
            this->status = IOStatus::InvalidIOStatus;
            this->actuallane = 0;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::RequestId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const {
            if (protId == 'IOPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetURL(const URL& val) {
            this->url = val;
        };
        const URL& GetURL() const {
            return this->url;
        };
        void SetLane(int32 val) {
            this->lane = val;
        };
        int32 GetLane() const {
            return this->lane;
        };
        void SetCacheReadEnabled(bool val) {
            this->cachereadenabled = val;
        };
        bool GetCacheReadEnabled() const {
            return this->cachereadenabled;
        };
        void SetCacheWriteEnabled(bool val) {
            this->cachewriteenabled = val;
        };
        bool GetCacheWriteEnabled() const {
            return this->cachewriteenabled;
        };
        void SetStartOffset(int32 val) {
            this->startoffset = val;
        };
        int32 GetStartOffset() const {
            return this->startoffset;
        };
        void SetEndOffset(int32 val) {
            this->endoffset = val;
        };
        int32 GetEndOffset() const {
            return this->endoffset;
        };
        void SetStatus(const IOStatus::Code& val) {
            this->status = val;
        };
        const IOStatus::Code& GetStatus() const {
            return this->status;
        };
        void SetErrorDesc(const String& val) {
            this->errordesc = val;
        };
        const String& GetErrorDesc() const {
            return this->errordesc;
        };
        void SetStream(const Ptr<Stream>& val) {
            this->stream = val;
        };
        const Ptr<Stream>& GetStream() const {
            return this->stream;
        };
        void SetActualLane(int32 val) {
            this->actuallane = val;
        };
        int32 GetActualLane() const {
            return this->actuallane;
        };
private:
        URL url;
        int32 lane;
        bool cachereadenabled;
        bool cachewriteenabled;
        int32 startoffset;
        int32 endoffset;
        IOStatus::Code status;
        String errordesc;
        Ptr<Stream> stream;
        int32 actuallane;
    };
    class notifyLanes : public Message {
        OryolClassPoolAllocDecl(notifyLanes);
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
        virtual bool IsMemberOf(ProtocolIdType protId) const {
            if (protId == 'IOPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetScheme(const StringAtom& val) {
            this->scheme = val;
        };
        const StringAtom& GetScheme() const {
            return this->scheme;
        };
private:
        StringAtom scheme;
    };
    class notifyFileSystemRemoved : public notifyLanes {
        OryolClassPoolAllocDecl(notifyFileSystemRemoved);
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
        virtual bool IsMemberOf(ProtocolIdType protId) const {
            if (protId == 'IOPT') return true;
            else return notifyLanes::IsMemberOf(protId);
        };
private:
    };
    class notifyFileSystemReplaced : public notifyLanes {
        OryolClassPoolAllocDecl(notifyFileSystemReplaced);
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
        virtual bool IsMemberOf(ProtocolIdType protId) const {
            if (protId == 'IOPT') return true;
            else return notifyLanes::IsMemberOf(protId);
        };
private:
    };
    class notifyFileSystemAdded : public notifyLanes {
        OryolClassPoolAllocDecl(notifyFileSystemAdded);
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
        virtual bool IsMemberOf(ProtocolIdType protId) const {
            if (protId == 'IOPT') return true;
            else return notifyLanes::IsMemberOf(protId);
        };
private:
    };
};
}
