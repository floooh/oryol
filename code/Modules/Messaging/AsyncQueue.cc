//------------------------------------------------------------------------------
//  AsyncQueue.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "AsyncQueue.h"

namespace Oryol {
namespace Messaging {
    
OryolClassPoolAllocImpl(AsyncQueue);

using namespace Core;

//------------------------------------------------------------------------------
AsyncQueue::AsyncQueue(const StringAtom& name_, int32 throttle_, int32 cap_) :
Port(name_),
throttle(throttle_),
cap(cap_) {
    // empty
}

//------------------------------------------------------------------------------
AsyncQueue::~AsyncQueue() {
    // FIXME: should we issue a warning if the queue is not empty?
    this->queue.Clear();
    this->forwardingPort = 0;
}

//------------------------------------------------------------------------------
void
AsyncQueue::SetThrottle(int32 num) {
    this->throttle = num;
}

//------------------------------------------------------------------------------
int32
AsyncQueue::GetThrottle() const {
    return this->throttle;
}

//------------------------------------------------------------------------------
void
AsyncQueue::SetCap(int32 num) {
    this->cap = num;
}

//------------------------------------------------------------------------------
int32
AsyncQueue::GetCap() const {
    return this->cap;
}

//------------------------------------------------------------------------------
void
AsyncQueue::SetForwardingPort(const Ptr<Port>& port) {
    this->forwardingPort = port;
}

//------------------------------------------------------------------------------
const Ptr<Port>&
AsyncQueue::GetForwardingPort() const {
    return this->forwardingPort;
}

//------------------------------------------------------------------------------
int32
AsyncQueue::GetNumQueuedMessages() const {
    return this->queue.Size();
}

//------------------------------------------------------------------------------
bool
AsyncQueue::Put(const Ptr<Message>& msg) {
    // check for message cap reached
    if ((this->cap > 0) && (this->queue.Size() >= this->cap)) {
        // cap reached
        return false;
    }
    else {
        // put message on queue
        this->queue.Enqueue(msg);
        return true;
    }
}

//------------------------------------------------------------------------------
int32
AsyncQueue::DoWork() {
    int32 numForwarded = 0;
    if (this->forwardingPort) {
        while (this->queue.Size() > 0) {
            
            // dequeue and forward message
            this->forwardingPort->Put(this->queue.Dequeue());
            numForwarded++;
        
            // check for cap reached
            if ((this->cap > 0) && (numForwarded == this->cap)) {
                return numForwarded;
            }
        }
    }
    return numForwarded;
}

} // namespace Messaging
} // namespace Oryol