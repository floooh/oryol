//-----------------------------------------------------------------------------
// #version:11# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "TestProtocol.h"

namespace Oryol {
OryolClassImpl(TestProtocol::TestMsg1);
OryolClassImpl(TestProtocol::TestMsg2);
OryolClassImpl(TestProtocol::TestArrayMsg);
TestProtocol::CreateCallback TestProtocol::jumpTable[TestProtocol::MessageId::NumMessageIds] = { 
    &TestProtocol::TestMsg1::FactoryCreate,
    &TestProtocol::TestMsg2::FactoryCreate,
    &TestProtocol::TestArrayMsg::FactoryCreate,
};
Ptr<Message>
TestProtocol::Factory::Create(MessageIdType id) {
    if (id < Protocol::MessageId::NumMessageIds) {
        return Protocol::Factory::Create(id);
    }
    else {
        o_assert(id < TestProtocol::MessageId::NumMessageIds);
        return jumpTable[id - Protocol::MessageId::NumMessageIds]();
    };
}
int32 TestProtocol::TestMsg1::EncodedSize() const {
    int32 s = Message::EncodedSize();
    s += Serializer::EncodedSize<int8>(this->int8val);
    s += Serializer::EncodedSize<int16>(this->int16val);
    s += Serializer::EncodedSize<int32>(this->int32val);
    s += Serializer::EncodedSize<int64>(this->int64val);
    s += Serializer::EncodedSize<uint8>(this->uint8val);
    s += Serializer::EncodedSize<uint16>(this->uint16val);
    s += Serializer::EncodedSize<uint32>(this->uint32val);
    s += Serializer::EncodedSize<uint64>(this->uint64val);
    s += Serializer::EncodedSize<float32>(this->float32val);
    s += Serializer::EncodedSize<float64>(this->float64val);
    return s;
}
uint8* TestProtocol::TestMsg1::Encode(uint8* dstPtr, const uint8* maxValidPtr) const {
    dstPtr = Message::Encode(dstPtr, maxValidPtr);
    dstPtr = Serializer::Encode<int8>(this->int8val, dstPtr, maxValidPtr);
    dstPtr = Serializer::Encode<int16>(this->int16val, dstPtr, maxValidPtr);
    dstPtr = Serializer::Encode<int32>(this->int32val, dstPtr, maxValidPtr);
    dstPtr = Serializer::Encode<int64>(this->int64val, dstPtr, maxValidPtr);
    dstPtr = Serializer::Encode<uint8>(this->uint8val, dstPtr, maxValidPtr);
    dstPtr = Serializer::Encode<uint16>(this->uint16val, dstPtr, maxValidPtr);
    dstPtr = Serializer::Encode<uint32>(this->uint32val, dstPtr, maxValidPtr);
    dstPtr = Serializer::Encode<uint64>(this->uint64val, dstPtr, maxValidPtr);
    dstPtr = Serializer::Encode<float32>(this->float32val, dstPtr, maxValidPtr);
    dstPtr = Serializer::Encode<float64>(this->float64val, dstPtr, maxValidPtr);
    return dstPtr;
}
const uint8* TestProtocol::TestMsg1::Decode(const uint8* srcPtr, const uint8* maxValidPtr) {
    srcPtr = Message::Decode(srcPtr, maxValidPtr);
    srcPtr = Serializer::Decode<int8>(srcPtr, maxValidPtr, this->int8val);
    srcPtr = Serializer::Decode<int16>(srcPtr, maxValidPtr, this->int16val);
    srcPtr = Serializer::Decode<int32>(srcPtr, maxValidPtr, this->int32val);
    srcPtr = Serializer::Decode<int64>(srcPtr, maxValidPtr, this->int64val);
    srcPtr = Serializer::Decode<uint8>(srcPtr, maxValidPtr, this->uint8val);
    srcPtr = Serializer::Decode<uint16>(srcPtr, maxValidPtr, this->uint16val);
    srcPtr = Serializer::Decode<uint32>(srcPtr, maxValidPtr, this->uint32val);
    srcPtr = Serializer::Decode<uint64>(srcPtr, maxValidPtr, this->uint64val);
    srcPtr = Serializer::Decode<float32>(srcPtr, maxValidPtr, this->float32val);
    srcPtr = Serializer::Decode<float64>(srcPtr, maxValidPtr, this->float64val);
    return srcPtr;
}
int32 TestProtocol::TestMsg2::EncodedSize() const {
    int32 s = TestMsg1::EncodedSize();
    s += Serializer::EncodedSize<String>(this->stringval);
    s += Serializer::EncodedSize<StringAtom>(this->stringatomval);
    return s;
}
uint8* TestProtocol::TestMsg2::Encode(uint8* dstPtr, const uint8* maxValidPtr) const {
    dstPtr = TestMsg1::Encode(dstPtr, maxValidPtr);
    dstPtr = Serializer::Encode<String>(this->stringval, dstPtr, maxValidPtr);
    dstPtr = Serializer::Encode<StringAtom>(this->stringatomval, dstPtr, maxValidPtr);
    return dstPtr;
}
const uint8* TestProtocol::TestMsg2::Decode(const uint8* srcPtr, const uint8* maxValidPtr) {
    srcPtr = TestMsg1::Decode(srcPtr, maxValidPtr);
    srcPtr = Serializer::Decode<String>(srcPtr, maxValidPtr, this->stringval);
    srcPtr = Serializer::Decode<StringAtom>(srcPtr, maxValidPtr, this->stringatomval);
    return srcPtr;
}
int32 TestProtocol::TestArrayMsg::EncodedSize() const {
    int32 s = Message::EncodedSize();
    s += Serializer::EncodedArraySize<int32>(this->int32arrayval);
    s += Serializer::EncodedArraySize<String>(this->stringarrayval);
    return s;
}
uint8* TestProtocol::TestArrayMsg::Encode(uint8* dstPtr, const uint8* maxValidPtr) const {
    dstPtr = Message::Encode(dstPtr, maxValidPtr);
    dstPtr = Serializer::EncodeArray<int32>(this->int32arrayval, dstPtr, maxValidPtr);
    dstPtr = Serializer::EncodeArray<String>(this->stringarrayval, dstPtr, maxValidPtr);
    return dstPtr;
}
const uint8* TestProtocol::TestArrayMsg::Decode(const uint8* srcPtr, const uint8* maxValidPtr) {
    srcPtr = Message::Decode(srcPtr, maxValidPtr);
    srcPtr = Serializer::DecodeArray<int32>(srcPtr, maxValidPtr, this->int32arrayval);
    srcPtr = Serializer::DecodeArray<String>(srcPtr, maxValidPtr, this->stringarrayval);
    return srcPtr;
}
}
