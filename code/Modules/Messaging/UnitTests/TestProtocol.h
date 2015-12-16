#pragma once
//-----------------------------------------------------------------------------
/* #version:14#
    machine generated, do not edit!
*/
#include <cstring>
#include "Messaging/Message.h"
#include "Messaging/Protocol.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/Array.h"

namespace Oryol {
class TestProtocol {
public:
    static ProtocolIdType GetProtocolId() {
        return 'TSTP';
    };
    class MessageId {
    public:
        enum {
            TestMsg1Id = Protocol::MessageId::NumMessageIds, 
            TestMsg2Id,
            TestArrayMsgId,
            NumMessageIds
        };
        static const char* ToString(MessageIdType c) {
            switch (c) {
                case TestMsg1Id: return "TestMsg1Id";
                case TestMsg2Id: return "TestMsg2Id";
                case TestArrayMsgId: return "TestArrayMsgId";
                default: return "InvalidMessageId";
            }
        };
        static MessageIdType FromString(const char* str) {
            if (std::strcmp("TestMsg1Id", str) == 0) return TestMsg1Id;
            if (std::strcmp("TestMsg2Id", str) == 0) return TestMsg2Id;
            if (std::strcmp("TestArrayMsgId", str) == 0) return TestArrayMsgId;
            return InvalidMessageId;
        };
    };
    typedef Ptr<Message> (*CreateCallback)();
    static CreateCallback jumpTable[TestProtocol::MessageId::NumMessageIds];
    class Factory {
    public:
        static Ptr<Message> Create(MessageIdType id);
    };
    class TestMsg1 : public Message {
        OryolClassDecl(TestMsg1);
        OryolTypeDecl(TestMsg1,Message);
    public:
        TestMsg1() {
            this->msgId = MessageId::TestMsg1Id;
            this->Int8Val = 0;
            this->Int16Val = -1;
            this->Int32Val = 0;
            this->Int64Val = 0;
            this->UInt8Val = 0;
            this->UInt16Val = 0;
            this->UInt32Val = 0;
            this->UInt64Val = 0;
            this->Float32Val = 123.0f;
            this->Float64Val = 12.0;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TestMsg1Id;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'TSTP') return true;
            else return Message::IsMemberOf(protId);
        };
        int8 Int8Val;
        int16 Int16Val;
        int32 Int32Val;
        int64 Int64Val;
        uint8 UInt8Val;
        uint16 UInt16Val;
        uint32 UInt32Val;
        uint64 UInt64Val;
        float32 Float32Val;
        float64 Float64Val;
    };
    class TestMsg2 : public TestMsg1 {
        OryolClassDecl(TestMsg2);
        OryolTypeDecl(TestMsg2,TestMsg1);
    public:
        TestMsg2() {
            this->msgId = MessageId::TestMsg2Id;
            this->StringVal = "Test";
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TestMsg2Id;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'TSTP') return true;
            else return TestMsg1::IsMemberOf(protId);
        };
        String StringVal;
        StringAtom StringAtomVal;
    };
    class TestArrayMsg : public Message {
        OryolClassDecl(TestArrayMsg);
        OryolTypeDecl(TestArrayMsg,Message);
    public:
        TestArrayMsg() {
            this->msgId = MessageId::TestArrayMsgId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TestArrayMsgId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'TSTP') return true;
            else return Message::IsMemberOf(protId);
        };
        Array<int32> Int32ArrayVal;
        Array<String> StringArrayVal;
    };
};
}
