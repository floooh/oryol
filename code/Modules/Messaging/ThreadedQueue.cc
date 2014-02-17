//------------------------------------------------------------------------------
//  ThreadedQueue.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ThreadedQueue.h"
#include "Core/Module.h"

namespace Oryol {
namespace Messaging {
    
OryolClassPoolAllocImpl(ThreadedQueue);

using namespace Core;

//------------------------------------------------------------------------------
/**
 NOTE: if the default constructor is used, a forwarding port must be 
 provided by the subclass in the onThreadStarted() method, or in the
 subclasses constructor!
*/
ThreadedQueue::ThreadedQueue() :
threadStarted(false),
threadStopRequested(false),
threadStopped(false) {
    this->createThreadId = std::this_thread::get_id();
}

//------------------------------------------------------------------------------
ThreadedQueue::ThreadedQueue(const Ptr<Port>& port_) :
forwardingPort(port_),
threadStarted(false),
threadStopRequested(false),
threadStopped(false) {
    this->createThreadId = std::this_thread::get_id();
}

//------------------------------------------------------------------------------
ThreadedQueue::~ThreadedQueue() {
    o_assert(this->isCreateThread());
    o_assert(this->threadStopped);
}

//------------------------------------------------------------------------------
void
ThreadedQueue::StartThread() {
    o_assert(this->isCreateThread());
    o_assert(!this->threadStarted);
    this->thread = std::thread(threadFunc, this);
    this->threadStarted = true;
}

//------------------------------------------------------------------------------
void
ThreadedQueue::StopThread() {
    o_assert(this->threadStarted);
    this->threadStopRequested = true;
    this->wakeup.notify_one();
    this->thread.join();
    this->threadStopped = true;
}

//------------------------------------------------------------------------------
bool
ThreadedQueue::Put(const Ptr<Message>& msg) {
    o_assert(this->isCreateThread());
    o_assert(this->threadStarted);
    o_assert(!this->threadStopped);
    this->writeQueue.Enqueue(msg);
    return true;
}

//------------------------------------------------------------------------------
void
ThreadedQueue::DoWork() {
    // move messages to transfer queue and wake up thread
    o_assert(this->isCreateThread());
    o_assert(this->threadStarted);
    o_assert(!this->threadStopped);
    if (!this->writeQueue.Empty()) {
        this->moveWriteToTransferQueue();
    }
    // signal the worker thread even if no messages have to be processed,
    // this is to prevent any messages getting stuck on the transfer queue
    this->wakeup.notify_one();
}

//------------------------------------------------------------------------------
bool
ThreadedQueue::isCreateThread() {
    return std::this_thread::get_id() == this->createThreadId;
}

//------------------------------------------------------------------------------
bool
ThreadedQueue::isWorkerThread() {
    return std::this_thread::get_id() == this->workThreadId;
}

//------------------------------------------------------------------------------
void
ThreadedQueue::moveWriteToTransferQueue() {
    o_assert(this->isCreateThread());
    // if the transfer queue is empty, we can do a very fast complete move
    this->transferQueueLock.lock();
    if (this->transferQueue.Empty()) {
        this->transferQueue = std::move(this->writeQueue);
    }
    else {
        // otherwise move messages one by one
        while (!this->writeQueue.Empty()) {
            this->transferQueue.Enqueue(std::move(this->writeQueue.Dequeue()));
        }
    }
    this->transferQueueLock.unlock();
}

//------------------------------------------------------------------------------
void
ThreadedQueue::moveTransferToReadQueue() {
    o_assert(this->isWorkerThread());
    o_assert(this->readQueue.Empty());
    this->transferQueueLock.lock();
    this->readQueue = std::move(this->transferQueue);
    this->transferQueueLock.unlock();
}

//------------------------------------------------------------------------------
void
ThreadedQueue::threadFunc(ThreadedQueue* self) {
    
    self->workThreadId = std::this_thread::get_id();
    
    // notify subclass that thread has been entered
    self->onThreadEnter();
    
    // the message processing loop waits for messages to arrive,
    // and forwards them to the forwardingPort
    while (!self->threadStopRequested) {

        // wait for messages to arrive, and if so, transfer to read queue
        std::unique_lock<std::mutex> lock(self->wakeupMutex);
        self->wakeup.wait(lock);
        self->moveTransferToReadQueue();
        lock.unlock();
        
        // now process the messages, this happens without locking
        while (!self->readQueue.Empty()) {
            self->onMessage(std::move(self->readQueue.Dequeue()));
        }
    }
    
    // notify subclass that we're about to leave the thread
    self->onThreadLeave();
}

//------------------------------------------------------------------------------
void
ThreadedQueue::onThreadEnter() {
    Core::Module::EnterThread();
}

//------------------------------------------------------------------------------
void
ThreadedQueue::onMessage(const Ptr<Message>& msg) {
    o_assert(this->forwardingPort.isValid());
    this->forwardingPort->Put(msg);
}

//------------------------------------------------------------------------------
void
ThreadedQueue::onThreadLeave() {
    Core::Module::LeaveThread();
}

} // namespace Messaging
} // namespace Oryol
