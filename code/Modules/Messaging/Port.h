#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Messaging::Port
    
    A Port is a generic Message handler. It receives a Message in the
    Put method and either somehow handles the message, or forwards it
    to one or more other Ports which are attached as Subscribers.
    
    Subclasses of Port implement specific message handling behaviour (such
    as invoking a handler callback, sending the message to a worker thread, 
    encoding the message into a buffer and sending it over a socket, and so on).
    Ports can be connected to message handling networks. 
    
    By default, Messages are forwarded through smart pointers, encoding/decoding
    will only happen when process boundaries are crossed.
*/
#include "Core/RefCounted.h"
#include "Core/String/StringAtom.h"
#include "Messaging/Message.h"

namespace Oryol {
namespace Messaging {

class Port : public Core::RefCounted {
    OryolClassDecl(Port);
public:
    /// construct with ProtocolId
    Port(const Core::StringAtom& name);
    /// destructor
    virtual ~Port();

    /// put a message into the port
    virtual void Put(const Core::Ptr<Message>& msg);
    /// get the port's name
    const Core::StringAtom& GetName() const;

protected:
    Core::StringAtom name;
};
} // namespace Messaging
} // namespace Oryol