//------------------------------------------------------------------------------
//  ThreadedQueue.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ThreadedQueue.h"

namespace Oryol {
namespace Messaging {
    
OryolClassPoolAllocImpl(ThreadedQueue);

using namespace Core;

//------------------------------------------------------------------------------
ThreadedQueue::ThreadedQueue(const StringAtom& name_, const Ptr<Port>& port_) :
Port(name_),
forwardingPort(port_) {
    
    // FIXME:
    // start the thread
}

//------------------------------------------------------------------------------
ThreadedQueue::~ThreadedQueue() {
    // FIXME:
    // - drop remaining messages(?)
    // -> or should we make sure to process them?
    // - join thread
}

//------------------------------------------------------------------------------
bool
ThreadedQueue::Put(const Ptr<Message>& msg) {
    this->writeQueue.Enqueue(msg);
    return true;
}

//------------------------------------------------------------------------------
void
ThreadedQueue::DoWork() {
    // FIXME:
    // lock transfer queue, if transfer queue is empty, move
    // write queue to transfer queue, otherwise move single messages
    // unlock transfer queue, signal condvar, done
}

} // namespace Messaging
} // namespace Oryol