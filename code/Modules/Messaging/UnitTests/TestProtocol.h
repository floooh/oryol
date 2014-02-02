#pragma once
//-----------------------------------------------------------------------------
/*
    machine generated, do not edit!
*/
#include "Messaging/Message.h"
#include "Messaging/Serializer.h"
#include "Messaging/MessageProtocol.h"
#include "Core/RefCounted.h"
#include "Core/Ptr.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"

namespace Oryol {
namespace TestProtocol {
class MessageId {
public:
    enum {
        TestMsg1Id = MessageProtocol::MessageId::NumMessageIds, 
        TestMsg2Id,
        TestArrayMsgId,
        NumMessageIds
    };
    static const char* ToString(Messaging::MessageIdType c) {
        switch (c) {
            case TestMsg1Id: return "TestMsg1Id";
            case TestMsg2Id: return "TestMsg2Id";
            case TestArrayMsgId: return "TestArrayMsgId";
            default: return "InvalidMessageId";
        }
    };
    static Messaging::MessageIdType FromString(const char* str) {
        if (std::strcmp("TestMsg1Id", str) == 0) return TestMsg1Id;
        if (std::strcmp("TestMsg2Id", str) == 0) return TestMsg2Id;
        if (std::strcmp("TestArrayMsgId", str) == 0) return TestArrayMsgId;
        return Messaging::InvalidMessageId;
    };
};
class TestMsg1 : public Messaging::Message {
    OryolClassDecl(TestMsg1);
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
        this->float32 = 123.0f;
        this->float64 = 12.0;
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
    void SetFloat32(float32 val) {
        this->float32 = val;
    };
    float32 GetFloat32() const {
        return this->float32;
    };
    void SetFloat64(float64 val) {
        this->float64 = val;
    };
    float64 GetFloat64() const {
        return this->float64;
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
    float32 float32;
    float64 float64;
};
class TestMsg2 : public TestMsg1 {
    OryolClassDecl(TestMsg2);
public:
    TestMsg2() {
        this->msgId = MessageId::TestMsg2Id;
        this->stringval = "Test";
    };
    void SetStringVal(const Core::String& val) {
        this->stringval = val;
    };
    const Core::String& GetStringVal() const {
        return this->stringval;
    };
    void SetStringAtomVal(const Core::StringAtom& val) {
        this->stringatomval = val;
    };
    const Core::StringAtom& GetStringAtomVal() const {
        return this->stringatomval;
    };
private:
    Core::String stringval;
    Core::StringAtom stringatomval;
};
class TestArrayMsg : public Messaging::Message {
    OryolClassDecl(TestArrayMsg);
public:
    TestArrayMsg() {
        this->msgId = MessageId::TestArrayMsgId;
    };
    void SetInt32ArrayVal(const Core::Array<int32>& val) {
        this->int32arrayval = val;
    };
    const Core::Array<int32>& GetInt32ArrayVal() const {
        return this->int32arrayval;
    };
    void SetStringArrayVal(const Core::Array<Core::String>& val) {
        this->stringarrayval = val;
    };
    const Core::Array<Core::String>& GetStringArrayVal() const {
        return this->stringarrayval;
    };
private:
    Core::Array<int32> int32arrayval;
    Core::Array<Core::String> stringarrayval;
};
}
}
