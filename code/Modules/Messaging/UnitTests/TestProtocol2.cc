//-----------------------------------------------------------------------------
// #version:5# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "TestProtocol2.h"

namespace Oryol {
OryolClassPoolAllocImpl(TestProtocol2::TestMsgEx);
TestProtocol2::CreateCallback TestProtocol2::jumpTable[TestProtocol2::MessageId::NumMessageIds] = { 
    &TestProtocol2::TestMsgEx::FactoryCreate,
};
Ptr<Message>
TestProtocol2::Factory::Create(MessageIdType id) {
    if (id < TestProtocol::MessageId::NumMessageIds) {
        return TestProtocol::Factory::Create(id);
    }
    else {
        o_assert(id < TestProtocol2::MessageId::NumMessageIds);
        return jumpTable[id - TestProtocol::MessageId::NumMessageIds]();
    };
}
int32 TestProtocol2::TestMsgEx::EncodedSize() const {
    int32 s = TestProtocol::TestMsg1::EncodedSize();
    s += Serializer::EncodedSize<int8>(this->exval2);
    return s;
}
uint8* TestProtocol2::TestMsgEx::Encode(uint8* dstPtr, const uint8* maxValidPtr) const {
    dstPtr = TestProtocol::TestMsg1::Encode(dstPtr, maxValidPtr);
    dstPtr = Serializer::Encode<int8>(this->exval2, dstPtr, maxValidPtr);
    return dstPtr;
}
const uint8* TestProtocol2::TestMsgEx::Decode(const uint8* srcPtr, const uint8* maxValidPtr) {
    srcPtr = TestProtocol::TestMsg1::Decode(srcPtr, maxValidPtr);
    srcPtr = Serializer::Decode<int8>(srcPtr, maxValidPtr, this->exval2);
    return srcPtr;
}
}
