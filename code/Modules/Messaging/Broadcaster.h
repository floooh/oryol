#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Broadcaster
    @ingroup Messaging
    @brief broadcast incoming messages to subscriber ports
    
    A Messaging Port which sends an incoming message to any number
    of subscriber Ports.
*/
#include "Messaging/Port.h"

namespace Oryol {
    
class Broadcaster : public Port {
    OryolClassPoolAllocDecl(Broadcaster);
public:
    /// constructor
    Broadcaster();
    /// destructor
    virtual ~Broadcaster();
    
    /// subscribe to messages from this port
    void Subscribe(const Ptr<Port>& port);
    /// unsubscribe from this port
    void Unsubscribe(const Ptr<Port>& port);
    /// get subscribers
    const Array<Ptr<Port>>& GetSubscribers() const;

    /// put a message into the port
    virtual bool Put(const Ptr<Message>& msg) override;
    /// perform work, this will be invoked on downstream ports
    virtual void DoWork();
    
protected:
    Array<Ptr<Port>> subscribers;
};
    
} // namespace Oryol
