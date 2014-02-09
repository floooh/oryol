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
AsyncQueue::~AsyncQueue() {
    // FIXME: should we issue a warning if the queue is not empty?
    this->queue.Clear();
    this->forwardingPort = 0;
}

//------------------------------------------------------------------------------
bool
AsyncQueue::Put(const Ptr<Message>& msg) {
    this->queue.Enqueue(msg);
    return true;
}

//------------------------------------------------------------------------------
void
AsyncQueue::DoWork() {
    if (this->forwardingPort) {
        while (!this->queue.Empty()) {
            this->forwardingPort->Put(this->queue.Dequeue());
        }
    }
    this->forwardingPort->DoWork();
}

} // namespace Messaging
} // namespace Oryol