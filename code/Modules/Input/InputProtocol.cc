//-----------------------------------------------------------------------------
// #version:14# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "InputProtocol.h"

namespace Oryol {
OryolClassImpl(InputProtocol::MouseMoveEvent);
OryolClassImpl(InputProtocol::MouseButtonEvent);
OryolClassImpl(InputProtocol::MouseScrollEvent);
OryolClassImpl(InputProtocol::KeyEvent);
OryolClassImpl(InputProtocol::WCharEvent);
OryolClassImpl(InputProtocol::TouchTappedEvent);
OryolClassImpl(InputProtocol::TouchDoubleTappedEvent);
OryolClassImpl(InputProtocol::TouchPanningStartedEvent);
OryolClassImpl(InputProtocol::TouchPanningEvent);
OryolClassImpl(InputProtocol::TouchPanningEndedEvent);
OryolClassImpl(InputProtocol::TouchPinchingStartedEvent);
OryolClassImpl(InputProtocol::TouchPinchingEvent);
OryolClassImpl(InputProtocol::TouchPinchingEndedEvent);
InputProtocol::CreateCallback InputProtocol::jumpTable[InputProtocol::MessageId::NumMessageIds] = { 
    &InputProtocol::MouseMoveEvent::FactoryCreate,
    &InputProtocol::MouseButtonEvent::FactoryCreate,
    &InputProtocol::MouseScrollEvent::FactoryCreate,
    &InputProtocol::KeyEvent::FactoryCreate,
    &InputProtocol::WCharEvent::FactoryCreate,
    &InputProtocol::TouchTappedEvent::FactoryCreate,
    &InputProtocol::TouchDoubleTappedEvent::FactoryCreate,
    &InputProtocol::TouchPanningStartedEvent::FactoryCreate,
    &InputProtocol::TouchPanningEvent::FactoryCreate,
    &InputProtocol::TouchPanningEndedEvent::FactoryCreate,
    &InputProtocol::TouchPinchingStartedEvent::FactoryCreate,
    &InputProtocol::TouchPinchingEvent::FactoryCreate,
    &InputProtocol::TouchPinchingEndedEvent::FactoryCreate,
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
