//-----------------------------------------------------------------------------
// #version:5# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "RenderProtocol.h"

namespace Oryol {
OryolClassPoolAllocImpl(RenderProtocol::DisplaySetup);
OryolClassPoolAllocImpl(RenderProtocol::DisplayDiscarded);
OryolClassPoolAllocImpl(RenderProtocol::DisplayModified);
RenderProtocol::CreateCallback RenderProtocol::jumpTable[RenderProtocol::MessageId::NumMessageIds] = { 
    &RenderProtocol::DisplaySetup::FactoryCreate,
    &RenderProtocol::DisplayDiscarded::FactoryCreate,
    &RenderProtocol::DisplayModified::FactoryCreate,
};
Ptr<Message>
RenderProtocol::Factory::Create(MessageIdType id) {
    if (id < Protocol::MessageId::NumMessageIds) {
        return Protocol::Factory::Create(id);
    }
    else {
        o_assert(id < RenderProtocol::MessageId::NumMessageIds);
        return jumpTable[id - Protocol::MessageId::NumMessageIds]();
    };
}
}
