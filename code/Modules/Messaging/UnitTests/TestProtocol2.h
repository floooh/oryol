#pragma once
//-----------------------------------------------------------------------------
/* #version:2#
    machine generated, do not edit!
*/
#include <cstring>
#include "Messaging/Message.h"
#include "Messaging/Serializer.h"
#include "Messaging/UnitTests/TestProtocol.h"

namespace Oryol {
namespace Messaging {
class TestProtocol2 {
public:
    static Messaging::ProtocolIdType GetProtocolId() {
        return 'TSP2';
    };
    class MessageId {
    public:
        enum {
            TestMsgExId = Messaging::TestProtocol::MessageId::NumMessageIds, 
            NumMessageIds
        };
        static const char* ToString(Messaging::MessageIdType c) {
            switch (c) {
                case TestMsgExId: return "TestMsgExId";
                default: return "InvalidMessageId";
            }
        };
        static Messaging::MessageIdType FromString(const char* str) {
            if (std::strcmp("TestMsgExId", str) == 0) return TestMsgExId;
            return Messaging::InvalidMessageId;
        };
    };
    typedef Core::Ptr<Messaging::Message> (*CreateCallback)();
    static CreateCallback jumpTable[TestProtocol2::MessageId::NumMessageIds];
    class Factory {
    public:
        static Core::Ptr<Messaging::Message> Create(Messaging::MessageIdType id);
    };
    class TestMsgEx : public Messaging::TestProtocol::TestMsg1 {
        OryolClassPoolAllocDecl(TestMsgEx);
    public:
        TestMsgEx() {
            this->msgId = MessageId::TestMsgExId;
            this->exval2 = 0;
        };
        static Core::Ptr<Messaging::Message> FactoryCreate() {
            return Create();
        };
        static Messaging::MessageIdType ClassMessageId() {
            return MessageId::TestMsgExId;
        };
        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {
            if (protId == 'TSP2') return true;
            else return Messaging::TestProtocol::TestMsg1::IsMemberOf(protId);
        };
        virtual int32 EncodedSize() const override;
        virtual uint8* Encode(uint8* dstPtr, const uint8* maxValidPtr) const override;
        virtual const uint8* Decode(const uint8* srcPtr, const uint8* maxValidPtr) override;
        void SetExVal2(int8 val) {
            this->exval2 = val;
        };
        int8 GetExVal2() const {
            return this->exval2;
        };
private:
        int8 exval2;
    };
};
}
}
