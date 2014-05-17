//-----------------------------------------------------------------------------
// #version:2# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "TestProtocol2.h"

namespace Oryol {
namespace Messaging {
OryolClassPoolAllocImpl(TestProtocol2::TestMsgEx);
TestProtocol2::CreateCallback TestProtocol2::jumpTable[TestProtocol2::MessageId::NumMessageIds] = { 
    &TestProtocol2::TestMsgEx::FactoryCreate,
};
Core::Ptr<Messaging::Message>
TestProtocol2::Factory::Create(Messaging::MessageIdType id) {
    if (id < Messaging::TestProtocol::MessageId::NumMessageIds) {
        return Messaging::TestProtocol::Factory::Create(id);
    }
    else {
        o_assert(id < TestProtocol2::MessageId::NumMessageIds);
        return jumpTable[id - Messaging::TestProtocol::MessageId::NumMessageIds]();
    };
}
int32 TestProtocol2::TestMsgEx::EncodedSize() const {
    int32 s = Messaging::TestProtocol::TestMsg1::EncodedSize();
    s += Messaging::Serializer::EncodedSize<int8>(this->exval2);
    return s;
}
uint8* TestProtocol2::TestMsgEx::Encode(uint8* dstPtr, const uint8* maxValidPtr) const {
    dstPtr = Messaging::TestProtocol::TestMsg1::Encode(dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<int8>(this->exval2, dstPtr, maxValidPtr);
    return dstPtr;
}
const uint8* TestProtocol2::TestMsgEx::Decode(const uint8* srcPtr, const uint8* maxValidPtr) {
    srcPtr = Messaging::TestProtocol::TestMsg1::Decode(srcPtr, maxValidPtr);
    srcPtr = Messaging::Serializer::Decode<int8>(srcPtr, maxValidPtr, this->exval2);
    return srcPtr;
}
}
}
