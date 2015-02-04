//-----------------------------------------------------------------------------
// #version:8# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "TestProtocol.h"

namespace Oryol {
OryolClassPoolAllocImpl(TestProtocol::TestMsg1);
OryolClassPoolAllocImpl(TestProtocol::TestMsg2);
OryolClassPoolAllocImpl(TestProtocol::TestArrayMsg);
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
}
