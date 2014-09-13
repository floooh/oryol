#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Port
    @ingroup Messaging
    @brief base class for message ports
    
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

class Port : public RefCounted {
    OryolClassDecl(Port);
public:
    /// construct with ProtocolId
    Port();
    /// destructor
    virtual ~Port();

    /// put a message into the port
    virtual bool Put(const Ptr<Message>& msg);
    /// perform work, this will be invoked on downstream ports
    virtual void DoWork();
};
} // namespace Oryol
