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
}
