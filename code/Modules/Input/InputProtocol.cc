//-----------------------------------------------------------------------------
// #version:11# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "InputProtocol.h"

namespace Oryol {
OryolClassImpl(InputProtocol::MouseMove);
OryolClassImpl(InputProtocol::MouseButton);
OryolClassImpl(InputProtocol::MouseScroll);
OryolClassImpl(InputProtocol::Key);
OryolClassImpl(InputProtocol::WChar);
OryolClassImpl(InputProtocol::TouchTapped);
OryolClassImpl(InputProtocol::TouchDoubleTapped);
OryolClassImpl(InputProtocol::TouchPanningStarted);
OryolClassImpl(InputProtocol::TouchPanning);
OryolClassImpl(InputProtocol::TouchPanningEnded);
OryolClassImpl(InputProtocol::TouchPinchingStarted);
OryolClassImpl(InputProtocol::TouchPinching);
OryolClassImpl(InputProtocol::TouchPinchingEnded);
InputProtocol::CreateCallback InputProtocol::jumpTable[InputProtocol::MessageId::NumMessageIds] = { 
    &InputProtocol::MouseMove::FactoryCreate,
    &InputProtocol::MouseButton::FactoryCreate,
    &InputProtocol::MouseScroll::FactoryCreate,
    &InputProtocol::Key::FactoryCreate,
    &InputProtocol::WChar::FactoryCreate,
    &InputProtocol::TouchTapped::FactoryCreate,
    &InputProtocol::TouchDoubleTapped::FactoryCreate,
    &InputProtocol::TouchPanningStarted::FactoryCreate,
    &InputProtocol::TouchPanning::FactoryCreate,
    &InputProtocol::TouchPanningEnded::FactoryCreate,
    &InputProtocol::TouchPinchingStarted::FactoryCreate,
    &InputProtocol::TouchPinching::FactoryCreate,
    &InputProtocol::TouchPinchingEnded::FactoryCreate,
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
