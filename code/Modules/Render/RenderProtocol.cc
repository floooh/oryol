//-----------------------------------------------------------------------------
// #version:2# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "RenderProtocol.h"

namespace Oryol {
namespace Render {
OryolClassPoolAllocImpl(RenderProtocol::DisplaySetup);
OryolClassPoolAllocImpl(RenderProtocol::DisplayDiscarded);
OryolClassPoolAllocImpl(RenderProtocol::DisplayModified);
RenderProtocol::CreateCallback RenderProtocol::jumpTable[RenderProtocol::MessageId::NumMessageIds] = { 
    &RenderProtocol::DisplaySetup::FactoryCreate,
    &RenderProtocol::DisplayDiscarded::FactoryCreate,
    &RenderProtocol::DisplayModified::FactoryCreate,
};
Core::Ptr<Messaging::Message>
RenderProtocol::Factory::Create(Messaging::MessageIdType id) {
    if (id < Messaging::Protocol::MessageId::NumMessageIds) {
        return Messaging::Protocol::Factory::Create(id);
    }
    else {
        o_assert(id < RenderProtocol::MessageId::NumMessageIds);
        return jumpTable[id - Messaging::Protocol::MessageId::NumMessageIds]();
    };
}
}
}
