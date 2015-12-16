#pragma once
//-----------------------------------------------------------------------------
/* #version:12#
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
            this->int8val = 0;
            this->int16val = -1;
            this->int32val = 0;
            this->int64val = 0;
            this->uint8val = 0;
            this->uint16val = 0;
            this->uint32val = 0;
            this->uint64val = 0;
            this->float32val = 123.0f;
            this->float64val = 12.0;
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
        void SetInt8Val(int8 val) {
            this->int8val = val;
        };
        int8 GetInt8Val() const {
            return this->int8val;
        };
        void SetInt16Val(int16 val) {
            this->int16val = val;
        };
        int16 GetInt16Val() const {
            return this->int16val;
        };
        void SetInt32Val(int32 val) {
            this->int32val = val;
        };
        int32 GetInt32Val() const {
            return this->int32val;
        };
        void SetInt64Val(int64 val) {
            this->int64val = val;
        };
        int64 GetInt64Val() const {
            return this->int64val;
        };
        void SetUInt8Val(uint8 val) {
            this->uint8val = val;
        };
        uint8 GetUInt8Val() const {
            return this->uint8val;
        };
        void SetUInt16Val(uint16 val) {
            this->uint16val = val;
        };
        uint16 GetUInt16Val() const {
            return this->uint16val;
        };
        void SetUInt32Val(uint32 val) {
            this->uint32val = val;
        };
        uint32 GetUInt32Val() const {
            return this->uint32val;
        };
        void SetUInt64Val(uint64 val) {
            this->uint64val = val;
        };
        uint64 GetUInt64Val() const {
            return this->uint64val;
        };
        void SetFloat32Val(float32 val) {
            this->float32val = val;
        };
        float32 GetFloat32Val() const {
            return this->float32val;
        };
        void SetFloat64Val(float64 val) {
            this->float64val = val;
        };
        float64 GetFloat64Val() const {
            return this->float64val;
        };
private:
        int8 int8val;
        int16 int16val;
        int32 int32val;
        int64 int64val;
        uint8 uint8val;
        uint16 uint16val;
        uint32 uint32val;
        uint64 uint64val;
        float32 float32val;
        float64 float64val;
    };
    class TestMsg2 : public TestMsg1 {
        OryolClassDecl(TestMsg2);
        OryolTypeDecl(TestMsg2,TestMsg1);
    public:
        TestMsg2() {
            this->msgId = MessageId::TestMsg2Id;
            this->stringval = "Test";
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
        void SetStringVal(const String& val) {
            this->stringval = val;
        };
        const String& GetStringVal() const {
            return this->stringval;
        };
        void SetStringAtomVal(const StringAtom& val) {
            this->stringatomval = val;
        };
        const StringAtom& GetStringAtomVal() const {
            return this->stringatomval;
        };
private:
        String stringval;
        StringAtom stringatomval;
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
        void SetInt32ArrayVal(const Array<int32>& val) {
            this->int32arrayval = val;
        };
        const Array<int32>& GetInt32ArrayVal() const {
            return this->int32arrayval;
        };
        void SetStringArrayVal(const Array<String>& val) {
            this->stringarrayval = val;
        };
        const Array<String>& GetStringArrayVal() const {
            return this->stringarrayval;
        };
private:
        Array<int32> int32arrayval;
        Array<String> stringarrayval;
    };
};
}
