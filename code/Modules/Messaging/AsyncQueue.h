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
    AsyncQueue(const Core::StringAtom& name);
    /// destructor
    virtual ~AsyncQueue();
    
    /// add the forwarding port, where messages are forwarded to in DoWork
    void SetForwardingPort(const Core::Ptr<Port>& port);
    /// get the forwarding port
    const Core::Ptr<Port>& GetForwardingPort() const;
    /// get number of queued messages
    int32 GetNumQueuedMessages() const;
    
    /// perform work, this will be invoked on downstream ports
    virtual void DoWork();
    /// put a message into the port
    virtual bool Put(const Core::Ptr<Message>& msg) override;

protected:
    Core::Queue<Core::Ptr<Message>> queue;
    Core::Ptr<Port> forwardingPort;
};

//------------------------------------------------------------------------------
inline
AsyncQueue::AsyncQueue(const Core::StringAtom& name_) :
Port(name_) {
    // empty
}

//------------------------------------------------------------------------------
inline void
AsyncQueue::SetForwardingPort(const Core::Ptr<Port>& port) {
    this->forwardingPort = port;
}

//------------------------------------------------------------------------------
inline const Core::Ptr<Port>&
AsyncQueue::GetForwardingPort() const {
    return this->forwardingPort;
}

//------------------------------------------------------------------------------
inline int32
AsyncQueue::GetNumQueuedMessages() const {
    return this->queue.Size();
}
    
} // namespace Messaging
} // namespace Oryol