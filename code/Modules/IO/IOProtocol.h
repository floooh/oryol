#pragma once
//-----------------------------------------------------------------------------
/* #version:2#
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
namespace IO {
class IOProtocol {
public:
    static Messaging::ProtocolIdType GetProtocolId() {
        return 'IOPT';
    };
    class MessageId {
    public:
        enum {
            RequestId = Messaging::Protocol::MessageId::NumMessageIds, 
            GetId,
            GetRangeId,
            notifyLanesId,
            notifyFileSystemRemovedId,
            notifyFileSystemReplacedId,
            notifyFileSystemAddedId,
            NumMessageIds
        };
        static const char* ToString(Messaging::MessageIdType c) {
            switch (c) {
                case RequestId: return "RequestId";
                case GetId: return "GetId";
                case GetRangeId: return "GetRangeId";
                case notifyLanesId: return "notifyLanesId";
                case notifyFileSystemRemovedId: return "notifyFileSystemRemovedId";
                case notifyFileSystemReplacedId: return "notifyFileSystemReplacedId";
                case notifyFileSystemAddedId: return "notifyFileSystemAddedId";
                default: return "InvalidMessageId";
            }
        };
        static Messaging::MessageIdType FromString(const char* str) {
            if (std::strcmp("RequestId", str) == 0) return RequestId;
            if (std::strcmp("GetId", str) == 0) return GetId;
            if (std::strcmp("GetRangeId", str) == 0) return GetRangeId;
            if (std::strcmp("notifyLanesId", str) == 0) return notifyLanesId;
            if (std::strcmp("notifyFileSystemRemovedId", str) == 0) return notifyFileSystemRemovedId;
            if (std::strcmp("notifyFileSystemReplacedId", str) == 0) return notifyFileSystemReplacedId;
            if (std::strcmp("notifyFileSystemAddedId", str) == 0) return notifyFileSystemAddedId;
            return Messaging::InvalidMessageId;
        };
    };
    typedef Core::Ptr<Messaging::Message> (*CreateCallback)();
    static CreateCallback jumpTable[IOProtocol::MessageId::NumMessageIds];
    class Factory {
    public:
        static Core::Ptr<Messaging::Message> Create(Messaging::MessageIdType id);
    };
    class Request : public Messaging::Message {
        OryolClassPoolAllocDecl(Request);
    public:
        Request() {
            this->msgId = MessageId::RequestId;
            this->lane = 0;
            this->cachereadenabled = false;
            this->cachewriteenabled = false;
            this->status = IOStatus::InvalidIOStatus;
        };
        static Core::Ptr<Messaging::Message> FactoryCreate() {
            return Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::RequestId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'IOPT') return true;
            else return Messaging::Message::IsMemberOf(protId);
        };
        void SetURL(const IO::URL& val) {
            this->url = val;
        };
        const IO::URL& GetURL() const {
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
        void SetStatus(const IOStatus::Code& val) {
            this->status = val;
        };
        const IOStatus::Code& GetStatus() const {
            return this->status;
        };
        void SetErrorDesc(const Core::String& val) {
            this->errordesc = val;
        };
        const Core::String& GetErrorDesc() const {
            return this->errordesc;
        };
private:
        IO::URL url;
        int32 lane;
        bool cachereadenabled;
        bool cachewriteenabled;
        IOStatus::Code status;
        Core::String errordesc;
    };
    class Get : public Request {
        OryolClassPoolAllocDecl(Get);
    public:
        Get() {
            this->msgId = MessageId::GetId;
        };
        static Core::Ptr<Messaging::Message> FactoryCreate() {
            return Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::GetId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'IOPT') return true;
            else return Request::IsMemberOf(protId);
        };
        void SetStream(const Core::Ptr<IO::MemoryStream>& val) {
            this->stream = val;
        };
        const Core::Ptr<IO::MemoryStream>& GetStream() const {
            return this->stream;
        };
private:
        Core::Ptr<IO::MemoryStream> stream;
    };
    class GetRange : public Get {
        OryolClassPoolAllocDecl(GetRange);
    public:
        GetRange() {
            this->msgId = MessageId::GetRangeId;
            this->startoffset = 0;
            this->endoffset = 0;
        };
        static Core::Ptr<Messaging::Message> FactoryCreate() {
            return Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::GetRangeId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'IOPT') return true;
            else return Get::IsMemberOf(protId);
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
private:
        int32 startoffset;
        int32 endoffset;
    };
    class notifyLanes : public Messaging::Message {
        OryolClassPoolAllocDecl(notifyLanes);
    public:
        notifyLanes() {
            this->msgId = MessageId::notifyLanesId;
        };
        static Core::Ptr<Messaging::Message> FactoryCreate() {
            return Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::notifyLanesId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'IOPT') return true;
            else return Messaging::Message::IsMemberOf(protId);
        };
private:
    };
    class notifyFileSystemRemoved : public notifyLanes {
        OryolClassPoolAllocDecl(notifyFileSystemRemoved);
    public:
        notifyFileSystemRemoved() {
            this->msgId = MessageId::notifyFileSystemRemovedId;
        };
        static Core::Ptr<Messaging::Message> FactoryCreate() {
            return Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::notifyFileSystemRemovedId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'IOPT') return true;
            else return notifyLanes::IsMemberOf(protId);
        };
        void SetScheme(const Core::StringAtom& val) {
            this->scheme = val;
        };
        const Core::StringAtom& GetScheme() const {
            return this->scheme;
        };
private:
        Core::StringAtom scheme;
    };
    class notifyFileSystemReplaced : public notifyLanes {
        OryolClassPoolAllocDecl(notifyFileSystemReplaced);
    public:
        notifyFileSystemReplaced() {
            this->msgId = MessageId::notifyFileSystemReplacedId;
        };
        static Core::Ptr<Messaging::Message> FactoryCreate() {
            return Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::notifyFileSystemReplacedId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'IOPT') return true;
            else return notifyLanes::IsMemberOf(protId);
        };
        void SetScheme(const Core::StringAtom& val) {
            this->scheme = val;
        };
        const Core::StringAtom& GetScheme() const {
            return this->scheme;
        };
private:
        Core::StringAtom scheme;
    };
    class notifyFileSystemAdded : public notifyLanes {
        OryolClassPoolAllocDecl(notifyFileSystemAdded);
    public:
        notifyFileSystemAdded() {
            this->msgId = MessageId::notifyFileSystemAddedId;
        };
        static Core::Ptr<Messaging::Message> FactoryCreate() {
            return Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::notifyFileSystemAddedId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'IOPT') return true;
            else return notifyLanes::IsMemberOf(protId);
        };
        void SetScheme(const Core::StringAtom& val) {
            this->scheme = val;
        };
        const Core::StringAtom& GetScheme() const {
            return this->scheme;
        };
private:
        Core::StringAtom scheme;
    };
};
}
}
