//-----------------------------------------------------------------------------
// machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "IOProtocol.h"

namespace Oryol {
namespace IO {
OryolClassPoolAllocImpl(IOProtocol::Request);
OryolClassPoolAllocImpl(IOProtocol::Get);
IOProtocol::CreateCallback IOProtocol::jumpTable[IOProtocol::MessageId::NumMessageIds] = { 
    &IOProtocol::Request::FactoryCreate,
    &IOProtocol::Get::FactoryCreate,
};
Messaging::Message*
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
