#pragma once
//-----------------------------------------------------------------------------
/* #version:14#
    machine generated, do not edit!
*/
#include <cstring>
#include "Messaging/Message.h"
#include "Messaging/UnitTests/TestProtocol.h"

namespace Oryol {
class TestProtocol2 {
public:
    static ProtocolIdType GetProtocolId() {
        return 'TSP2';
    };
    class MessageId {
    public:
        enum {
            TestMsgExId = TestProtocol::MessageId::NumMessageIds, 
            NumMessageIds
        };
        static const char* ToString(MessageIdType c) {
            switch (c) {
                case TestMsgExId: return "TestMsgExId";
                default: return "InvalidMessageId";
            }
        };
        static MessageIdType FromString(const char* str) {
            if (std::strcmp("TestMsgExId", str) == 0) return TestMsgExId;
            return InvalidMessageId;
        };
    };
    typedef Ptr<Message> (*CreateCallback)();
    static CreateCallback jumpTable[TestProtocol2::MessageId::NumMessageIds];
    class Factory {
    public:
        static Ptr<Message> Create(MessageIdType id);
    };
    class TestMsgEx : public TestProtocol::TestMsg1 {
        OryolClassDecl(TestMsgEx);
        OryolTypeDecl(TestMsgEx,TestProtocol::TestMsg1);
    public:
        TestMsgEx() {
            this->msgId = MessageId::TestMsgExId;
            this->ExVal2 = 0;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TestMsgExId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'TSP2') return true;
            else return TestProtocol::TestMsg1::IsMemberOf(protId);
        };
        int8_t ExVal2;
    };
};
}
