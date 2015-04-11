//-----------------------------------------------------------------------------
// #version:8# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "InputProtocol.h"

namespace Oryol {
OryolClassPoolAllocImpl(InputProtocol::MouseMove);
OryolClassPoolAllocImpl(InputProtocol::MouseButton);
OryolClassPoolAllocImpl(InputProtocol::MouseScroll);
OryolClassPoolAllocImpl(InputProtocol::Key);
OryolClassPoolAllocImpl(InputProtocol::WChar);
InputProtocol::CreateCallback InputProtocol::jumpTable[InputProtocol::MessageId::NumMessageIds] = { 
    &InputProtocol::MouseMove::FactoryCreate,
    &InputProtocol::MouseButton::FactoryCreate,
    &InputProtocol::MouseScroll::FactoryCreate,
    &InputProtocol::Key::FactoryCreate,
    &InputProtocol::WChar::FactoryCreate,
};
Ptr<Message>
InputProtocol::Factory::Create(MessageIdType id) {
    if (id < Protocol::MessageId::NumMessageIds) {
        return Protocol::Factory::Create(id);
    }
    else {
        o_assert(id < InputProtocol::MessageId::NumMessageIds);
        return jumpTable[id - Protocol::MessageId::NumMessageIds]();
    };
}
}
