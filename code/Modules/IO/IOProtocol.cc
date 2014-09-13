//-----------------------------------------------------------------------------
// #version:5# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "IOProtocol.h"

namespace Oryol {
OryolClassPoolAllocImpl(IOProtocol::Request);
OryolClassPoolAllocImpl(IOProtocol::Get);
OryolClassPoolAllocImpl(IOProtocol::GetRange);
OryolClassPoolAllocImpl(IOProtocol::notifyLanes);
OryolClassPoolAllocImpl(IOProtocol::notifyFileSystemRemoved);
OryolClassPoolAllocImpl(IOProtocol::notifyFileSystemReplaced);
OryolClassPoolAllocImpl(IOProtocol::notifyFileSystemAdded);
IOProtocol::CreateCallback IOProtocol::jumpTable[IOProtocol::MessageId::NumMessageIds] = { 
    &IOProtocol::Request::FactoryCreate,
    &IOProtocol::Get::FactoryCreate,
    &IOProtocol::GetRange::FactoryCreate,
    &IOProtocol::notifyLanes::FactoryCreate,
    &IOProtocol::notifyFileSystemRemoved::FactoryCreate,
    &IOProtocol::notifyFileSystemReplaced::FactoryCreate,
    &IOProtocol::notifyFileSystemAdded::FactoryCreate,
};
Ptr<Message>
IOProtocol::Factory::Create(MessageIdType id) {
    if (id < Protocol::MessageId::NumMessageIds) {
        return Protocol::Factory::Create(id);
    }
    else {
        o_assert(id < IOProtocol::MessageId::NumMessageIds);
        return jumpTable[id - Protocol::MessageId::NumMessageIds]();
    };
}
}
