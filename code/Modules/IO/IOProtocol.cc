//-----------------------------------------------------------------------------
// #version:2# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "IOProtocol.h"

namespace Oryol {
namespace IO {
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
Core::Ptr<Messaging::Message>
IOProtocol::Factory::Create(Messaging::MessageIdType id) {
    if (id < Messaging::Protocol::MessageId::NumMessageIds) {
        return Messaging::Protocol::Factory::Create(id);
    }
    else {
        o_assert(id < IOProtocol::MessageId::NumMessageIds);
        return jumpTable[id - Messaging::Protocol::MessageId::NumMessageIds]();
    };
}
}
}
