#pragma once
//-----------------------------------------------------------------------------
/*
    machine generated, do not edit!
*/
#include "Messaging/Message.h"
#include "Messaging/Serializer.h"
#include "Messaging/UnitTests/TestProtocol.h"

namespace Oryol {
namespace TestProtocol2 {
class MessageId {
public:
    enum {
        TestMsgExId = TestProtocol::MessageId::NumMessageIds, 
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
class TestMsgEx : public TestProtocol::TestMsg1 {
    OryolClassPoolAllocDecl(TestMsgEx, 128);
public:
    TestMsgEx() {
        this->msgId = MessageId::TestMsgExId;
        this->exval2 = 0;
    };
    void SetExVal2(int8 val) {
        this->exval2 = val;
    };
    int8 GetExVal2() const {
        return this->exval2;
    };
private:
    int8 exval2;
};
}
}
