//-----------------------------------------------------------------------------
// #version:11# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "TestProtocol2.h"

namespace Oryol {
OryolClassImpl(TestProtocol2::TestMsgEx);
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
}
