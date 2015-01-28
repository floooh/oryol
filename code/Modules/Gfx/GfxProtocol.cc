//-----------------------------------------------------------------------------
// #version:7# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "GfxProtocol.h"

namespace Oryol {
OryolClassPoolAllocImpl(GfxProtocol::DisplaySetup);
OryolClassPoolAllocImpl(GfxProtocol::DisplayDiscarded);
OryolClassPoolAllocImpl(GfxProtocol::DisplayModified);
GfxProtocol::CreateCallback GfxProtocol::jumpTable[GfxProtocol::MessageId::NumMessageIds] = { 
    &GfxProtocol::DisplaySetup::FactoryCreate,
    &GfxProtocol::DisplayDiscarded::FactoryCreate,
    &GfxProtocol::DisplayModified::FactoryCreate,
};
Ptr<Message>
GfxProtocol::Factory::Create(MessageIdType id) {
    if (id < Protocol::MessageId::NumMessageIds) {
        return Protocol::Factory::Create(id);
    }
    else {
        o_assert(id < GfxProtocol::MessageId::NumMessageIds);
        return jumpTable[id - Protocol::MessageId::NumMessageIds]();
    };
}
}
