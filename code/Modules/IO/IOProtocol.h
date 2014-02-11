#pragma once
//-----------------------------------------------------------------------------
/*
    machine generated, do not edit!
*/
#include <cstring>
#include "Messaging/Message.h"
#include "Messaging/Serializer.h"
#include "Messaging/Protocol.h"
#include "Core/Ptr.h"
#include "IO/URL.h"
#include "IO/MemoryStream.h"

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
            NumMessageIds
        };
        static const char* ToString(Messaging::MessageIdType c) {
            switch (c) {
                case RequestId: return "RequestId";
                case GetId: return "GetId";
                default: return "InvalidMessageId";
            }
        };
        static Messaging::MessageIdType FromString(const char* str) {
            if (std::strcmp("RequestId", str) == 0) return RequestId;
            if (std::strcmp("GetId", str) == 0) return GetId;
            return Messaging::InvalidMessageId;
        };
    };
    typedef Messaging::Message* (*CreateCallback)();
    static CreateCallback jumpTable[IOProtocol::MessageId::NumMessageIds];
    class Factory {
    public:
        static Messaging::Message* Create(Messaging::MessageIdType id);
    };
    class Request : public Messaging::Message {
        OryolClassPoolAllocDecl(Request);
    public:
        Request() {
            this->msgId = MessageId::RequestId;
            this->channel = 0;
            this->cachereadenabled = false;
            this->cachewriteenabled = false;
            this->result = 0;
        };
        static Messaging::Message* FactoryCreate() {
            return (Messaging::Message*) Create();
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
        void SetChannel(int32 val) {
            this->channel = val;
        };
        int32 GetChannel() const {
            return this->channel;
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
        void SetResult(int32 val) {
            this->result = val;
        };
        int32 GetResult() const {
            return this->result;
        };
private:
        IO::URL url;
        int32 channel;
        bool cachereadenabled;
        bool cachewriteenabled;
        int32 result;
    };
    class Get : public Messaging::Message {
        OryolClassPoolAllocDecl(Get);
    public:
        Get() {
            this->msgId = MessageId::GetId;
            this->rangestartindex = InvalidIndex;
            this->rangeendindex = InvalidIndex;
        };
        static Messaging::Message* FactoryCreate() {
            return (Messaging::Message*) Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::GetId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'IOPT') return true;
            else return Messaging::Message::IsMemberOf(protId);
        };
        void SetRangeStartIndex(int32 val) {
            this->rangestartindex = val;
        };
        int32 GetRangeStartIndex() const {
            return this->rangestartindex;
        };
        void SetRangeEndIndex(int32 val) {
            this->rangeendindex = val;
        };
        int32 GetRangeEndIndex() const {
            return this->rangeendindex;
        };
        void SetStream(const Core::Ptr<IO::MemoryStream>& val) {
            this->stream = val;
        };
        const Core::Ptr<IO::MemoryStream>& GetStream() const {
            return this->stream;
        };
private:
        int32 rangestartindex;
        int32 rangeendindex;
        Core::Ptr<IO::MemoryStream> stream;
    };
};
}
}
