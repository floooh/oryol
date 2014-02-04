//-----------------------------------------------------------------------------
// machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "TestProtocol2.h"

namespace Oryol {
namespace TestProtocol2 {
OryolClassPoolAllocImpl(TestMsgEx, 128);
typedef Messaging::Message* (*CreateCallback)();
CreateCallback jumpTable[MessageId::NumMessageIds] = { 
    &TestMsgEx::FactoryCreate,
};
Messaging::Message*
Factory::Create(Messaging::MessageIdType id) {
    if (id < TestProtocol::MessageId::NumMessageIds) {
        return TestProtocol::Factory::Create(id);
    }
    else {
        o_assert(id < TestProtocol2::MessageId::NumMessageIds);
        return jumpTable[id - TestProtocol::MessageId::NumMessageIds]();
    };
}
int32 TestMsgEx::EncodedSize() const {
    int32 s = TestProtocol::TestMsg1::EncodedSize();
    s += Messaging::Serializer::EncodedSize<int8>(this->exval2);
    return s;
}
uint8* TestMsgEx::Encode(uint8* dstPtr, const uint8* maxValidPtr) const {
    dstPtr = TestProtocol::TestMsg1::Encode(dstPtr, maxValidPtr);
    dstPtr = Messaging::Serializer::Encode<int8>(this->exval2, dstPtr, maxValidPtr);
    return dstPtr;
}
const uint8* TestMsgEx::Decode(const uint8* srcPtr, const uint8* maxValidPtr) {
    srcPtr = TestProtocol::TestMsg1::Decode(srcPtr, maxValidPtr);
    srcPtr = Messaging::Serializer::Decode<int8>(srcPtr, maxValidPtr, this->exval2);
    return srcPtr;
}
}
}
