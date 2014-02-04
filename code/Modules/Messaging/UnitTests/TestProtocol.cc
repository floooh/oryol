//-----------------------------------------------------------------------------
// machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "TestProtocol.h"

namespace Oryol {
namespace TestProtocol {
OryolClassImpl(TestMsg1);
OryolClassImpl(TestMsg2);
OryolClassImpl(TestArrayMsg);
typedef Messaging::Message* (*CreateCallback)();
CreateCallback jumpTable[MessageId::NumMessageIds] = { 
    &TestMsg1::FactoryCreate,
    &TestMsg2::FactoryCreate,
    &TestArrayMsg::FactoryCreate,
};
Messaging::Message*
Factory::Create(Messaging::MessageIdType id) {
    if (id < MessageProtocol::MessageId::NumMessageIds) {
        return MessageProtocol::Factory::Create(id);
    }
    else {
        o_assert(id < TestProtocol::MessageId::NumMessageIds);
        return jumpTable[id - MessageProtocol::MessageId::NumMessageIds]();
    };
}
int32 TestMsg1::EncodedSize() const {
    int32 s = Messaging::Message::EncodedSize();
    s += Messaging::Serializer::EncodedSize<int8>(this->int8val);
    s += Messaging::Serializer::EncodedSize<int16>(this->int16val);
    s += Messaging::Serializer::EncodedSize<int32>(this->int32val);
    s += Messaging::Serializer::EncodedSize<int64>(this->int64val);
    s += Messaging::Serializer::EncodedSize<uint8>(this->uint8val);
    s += Messaging::Serializer::EncodedSize<uint16>(this->uint16val);
    s += Messaging::Serializer::EncodedSize<uint32>(this->uint32val);
    s += Messaging::Serializer::EncodedSize<uint64>(this->uint64val);
    s += Messaging::Serializer::EncodedSize<float32>(this->float32val);
    s += Messaging::Serializer::EncodedSize<float64>(this->float64val);
    return s;
}
uint8* TestMsg1::Encode(uint8* dstPtr, const uint8* maxValidPtr) const {
    dstPtr = Messaging::Message::Encode(dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<int8>(this->int8val, dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<int16>(this->int16val, dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<int32>(this->int32val, dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<int64>(this->int64val, dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<uint8>(this->uint8val, dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<uint16>(this->uint16val, dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<uint32>(this->uint32val, dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<uint64>(this->uint64val, dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<float32>(this->float32val, dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<float64>(this->float64val, dstPtr, maxValidPtr);
    return dstPtr;
}
const uint8* TestMsg1::Decode(const uint8* srcPtr, const uint8* maxValidPtr) {
    srcPtr = Messaging::Message::Decode(srcPtr, maxValidPtr);
    srcPtr = Messaging::Serializer::Decode<int8>(srcPtr, maxValidPtr, this->int8val);
    srcPtr = Messaging::Serializer::Decode<int16>(srcPtr, maxValidPtr, this->int16val);
    srcPtr = Messaging::Serializer::Decode<int32>(srcPtr, maxValidPtr, this->int32val);
    srcPtr = Messaging::Serializer::Decode<int64>(srcPtr, maxValidPtr, this->int64val);
    srcPtr = Messaging::Serializer::Decode<uint8>(srcPtr, maxValidPtr, this->uint8val);
    srcPtr = Messaging::Serializer::Decode<uint16>(srcPtr, maxValidPtr, this->uint16val);
    srcPtr = Messaging::Serializer::Decode<uint32>(srcPtr, maxValidPtr, this->uint32val);
    srcPtr = Messaging::Serializer::Decode<uint64>(srcPtr, maxValidPtr, this->uint64val);
    srcPtr = Messaging::Serializer::Decode<float32>(srcPtr, maxValidPtr, this->float32val);
    srcPtr = Messaging::Serializer::Decode<float64>(srcPtr, maxValidPtr, this->float64val);
    return srcPtr;
}
int32 TestMsg2::EncodedSize() const {
    int32 s = TestMsg1::EncodedSize();
    s += Messaging::Serializer::EncodedSize<Core::String>(this->stringval);
    s += Messaging::Serializer::EncodedSize<Core::StringAtom>(this->stringatomval);
    return s;
}
uint8* TestMsg2::Encode(uint8* dstPtr, const uint8* maxValidPtr) const {
    dstPtr = TestMsg1::Encode(dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<Core::String>(this->stringval, dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<Core::StringAtom>(this->stringatomval, dstPtr, maxValidPtr);
    return dstPtr;
}
const uint8* TestMsg2::Decode(const uint8* srcPtr, const uint8* maxValidPtr) {
    srcPtr = TestMsg1::Decode(srcPtr, maxValidPtr);
    srcPtr = Messaging::Serializer::Decode<Core::String>(srcPtr, maxValidPtr, this->stringval);
    srcPtr = Messaging::Serializer::Decode<Core::StringAtom>(srcPtr, maxValidPtr, this->stringatomval);
    return srcPtr;
}
int32 TestArrayMsg::EncodedSize() const {
    int32 s = Messaging::Message::EncodedSize();
    s += Messaging::Serializer::EncodedArraySize<int32>(this->int32arrayval);
    s += Messaging::Serializer::EncodedArraySize<Core::String>(this->stringarrayval);
    return s;
}
uint8* TestArrayMsg::Encode(uint8* dstPtr, const uint8* maxValidPtr) const {
    dstPtr = Messaging::Message::Encode(dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::EncodeArray<int32>(this->int32arrayval, dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::EncodeArray<Core::String>(this->stringarrayval, dstPtr, maxValidPtr);
    return dstPtr;
}
const uint8* TestArrayMsg::Decode(const uint8* srcPtr, const uint8* maxValidPtr) {
    srcPtr = Messaging::Message::Decode(srcPtr, maxValidPtr);
    srcPtr = Messaging::Serializer::DecodeArray<int32>(srcPtr, maxValidPtr, this->int32arrayval);
    srcPtr = Messaging::Serializer::DecodeArray<Core::String>(srcPtr, maxValidPtr, this->stringarrayval);
    return srcPtr;
}
}
}
