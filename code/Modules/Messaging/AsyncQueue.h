#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Messaging::AsyncQueue
    @brief single-threaded asynchronous message queue
    
    A Port which acts as a single-threaded, asynchronous, message queue.
    Incoming messages are put on a Queue, and are forwarded to the
    attached Port when DoWork() is called.
*/
#include "Messaging/Port.h"
#include "Core/Containers/Queue.h"

namespace Oryol {
namespace Messaging {
    
class AsyncQueue : public Port {
    OryolClassPoolAllocDecl(AsyncQueue);
public:
    /// constructor
    AsyncQueue();
    /// destructor
    virtual ~AsyncQueue();
    
    /// add the forwarding port, where messages are forwarded to in DoWork
    void SetForwardingPort(const Core::Ptr<Port>& port);
    /// get the forwarding port
    const Core::Ptr<Port>& GetForwardingPort() const;
    /// get number of queued messages
    int32 GetNumQueuedMessages() const;
    
    /// this only forwards the DoWork() call to the forwarding port
    virtual void DoWork();
    /// put a message into the port
    virtual bool Put(const Core::Ptr<Message>& msg) override;
    /// explicitely forward queued messages
    void ForwardMessages();

protected:
    Core::Queue<Core::Ptr<Message>> queue;
    Core::Ptr<Port> forwardingPort;
};

} // namespace Messaging
} // namespace Oryol