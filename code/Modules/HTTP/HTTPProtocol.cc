//-----------------------------------------------------------------------------
// machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "HTTPProtocol.h"

namespace Oryol {
namespace HTTP {
OryolClassPoolAllocImpl(HTTPProtocol::HTTPRequest);
HTTPProtocol::CreateCallback HTTPProtocol::jumpTable[HTTPProtocol::MessageId::NumMessageIds] = { 
    &HTTPProtocol::HTTPRequest::FactoryCreate,
};
Messaging::Message*
HTTPProtocol::Factory::Create(Messaging::MessageIdType id) {
    if (id < Messaging::Protocol::MessageId::NumMessageIds) {
        return Messaging::Protocol::Factory::Create(id);
    }
    else {
        o_assert(id < HTTPProtocol::MessageId::NumMessageIds);
        return jumpTable[id - Messaging::Protocol::MessageId::NumMessageIds]();
    };
}
}
}
