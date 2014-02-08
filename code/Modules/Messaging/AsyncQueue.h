#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Messaging::AsyncQueue
    
    A Port which acts as a single-threaded, asynchronous, message queue.
    Incoming messages are put on a Queue, and are forwarded to the
    attached Port when DoWork() is called.
    
    The Queue can be throttled (only process N messages per call to
    DoWork), and capped (don't allow more then N messages to queue up.
*/
#include "Messaging/Port.h"
#include "Core/Containers/Queue.h"

namespace Oryol {
namespace Messaging {
    
class AsyncQueue : public Port {
    OryolClassPoolAllocDecl(AsyncQueue);
public:
    /// constructor
    AsyncQueue(const Core::StringAtom& name, int32 throttle=0, int32 cap=0);
    /// destructor
    virtual ~AsyncQueue();
    
    /// set throttle value (number of messages forwarded per DoWork, 0 means unthrottled)
    void SetThrottle(int32 num);
    /// get throttle value
    int32 GetThrottle() const;
    /// set cap value (max number of pending messages, 0 is uncapped)
    void SetCap(int32 num);
    /// get cap value
    int32 GetCap() const;
    /// add the forwarding port, where messages are forwarded to in DoWork
    void SetForwardingPort(const Core::Ptr<Port>& port);
    /// get the forwarding port
    const Core::Ptr<Port>& GetForwardingPort() const;
    /// get number of queued messages
    int32 GetNumQueuedMessages() const;
    
    /// process queued messages (at most 'throttle' num messages), return number of forwarded messages
    int32 DoWork();
    
    /// put a message into the port
    virtual bool Put(const Core::Ptr<Message>& msg) override;

protected:
    int32 throttle;
    int32 cap;
    Core::Queue<Core::Ptr<Message>> queue;
    Core::Ptr<Port> forwardingPort;
};
    
} // namespace Messaging
} // namespace Oryol