#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Messaging::Broadcaster
    
    A Messaging Port which sends an incoming message to any number
    of subscriber Ports.
*/
#include "Messaging/Port.h"

namespace Oryol {
namespace Messaging {
    
class Broadcaster : public Port {
    OryolClassPoolAllocDecl(Broadcaster);
public:
    /// constructor
    Broadcaster(const Core::StringAtom& name);
    /// destructor
    virtual ~Broadcaster();
    
    /// put a message into the port
    virtual void Put(const Core::Ptr<Message>& msg);

    /// subscribe to messages from this port
    void Subscribe(const Core::Ptr<Port>& port);
    /// unsubscribe from this port
    void Unsubscribe(const Core::Ptr<Port>& port);
    /// get subscribers
    const Core::Array<Core::Ptr<Port>>& GetSubscribers() const;

protected:
    Core::Array<Core::Ptr<Port>> subscribers;
};
    
} // namespace Messaging
} // namespace Oryol