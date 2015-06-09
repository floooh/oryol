//-----------------------------------------------------------------------------
// #version:11# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "HTTPProtocol.h"

namespace Oryol {
OryolClassImpl(HTTPProtocol::HTTPResponse);
OryolClassImpl(HTTPProtocol::HTTPRequest);
HTTPProtocol::CreateCallback HTTPProtocol::jumpTable[HTTPProtocol::MessageId::NumMessageIds] = { 
    &HTTPProtocol::HTTPResponse::FactoryCreate,
    &HTTPProtocol::HTTPRequest::FactoryCreate,
};
Ptr<Message>
HTTPProtocol::Factory::Create(MessageIdType id) {
    if (id < Protocol::MessageId::NumMessageIds) {
        return Protocol::Factory::Create(id);
    }
    else {
        o_assert(id < HTTPProtocol::MessageId::NumMessageIds);
        return jumpTable[id - Protocol::MessageId::NumMessageIds]();
    };
}
}
