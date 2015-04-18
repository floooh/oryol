//-----------------------------------------------------------------------------
// #version:10# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "IOProtocol.h"

namespace Oryol {
OryolClassImpl(IOProtocol::Request);
OryolClassImpl(IOProtocol::notifyLanes);
OryolClassImpl(IOProtocol::notifyFileSystemRemoved);
OryolClassImpl(IOProtocol::notifyFileSystemReplaced);
OryolClassImpl(IOProtocol::notifyFileSystemAdded);
IOProtocol::CreateCallback IOProtocol::jumpTable[IOProtocol::MessageId::NumMessageIds] = { 
    &IOProtocol::Request::FactoryCreate,
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
