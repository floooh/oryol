//------------------------------------------------------------------------------
//  ThreadedQueue.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ThreadedQueue.h"
#include "Core/CoreFacade.h"

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
tickDuration(0),
threadStarted(false),
threadStopRequested(false),
threadStopped(false) {
    #if ORYOL_HAS_THREADS
        this->createThreadId = std::this_thread::get_id();
    #endif
}

//------------------------------------------------------------------------------
ThreadedQueue::ThreadedQueue(const Ptr<Port>& port_) :
tickDuration(0),
forwardingPort(port_),
threadStarted(false),
threadStopRequested(false),
threadStopped(false) {
    #if ORYOL_HAS_THREADS
        this->createThreadId = std::this_thread::get_id();
    #endif
}

//------------------------------------------------------------------------------
ThreadedQueue::~ThreadedQueue() {
    o_assert(this->isCreateThread());
    o_assert(this->threadStopped);
}

//------------------------------------------------------------------------------
/**
 If a tick-duration is set, the thread will wakeup after this duration
 even when no messages are pending. This is useful to do some work
 which doesn't depend on incoming messages. A tick duration of 0 is the
 default and means that the thread will NOT tick (e.g. only wake up
 when messages arrive)
*/
void
ThreadedQueue::SetTickDuration(uint32 milliSec) {
    o_assert(!this->threadStarted);
    this->tickDuration = milliSec;
}

//------------------------------------------------------------------------------
uint32
ThreadedQueue::GetTickDuration() const {
    return this->tickDuration;
}

//------------------------------------------------------------------------------
void
ThreadedQueue::StartThread() {
    o_assert(this->isCreateThread());
    o_assert(!this->threadStarted);
    #if ORYOL_HAS_THREADS
        this->thread = std::thread(threadFunc, this);
    #else
        this->onThreadEnter();
    #endif
    this->threadStarted = true;
}

//------------------------------------------------------------------------------
void
ThreadedQueue::StopThread() {
    o_assert(this->threadStarted);
    this->threadStopRequested = true;
    #if ORYOL_HAS_THREADS
        this->wakeup.notify_one();
        this->thread.join();
    #else
        this->onThreadLeave();
    #endif
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
    #if ORYOL_HAS_THREADS
        this->wakeup.notify_one();
    #else
        // if no threads are available, we pump the message queue right
        // here
        // FIXME: we could do without all those queue transfers here!
        this->moveTransferToReadQueue();
        while (!this->readQueue.Empty()) {
            this->onMessage(std::move(this->readQueue.Dequeue()));
        }
        this->onTick();
    #endif
}

//------------------------------------------------------------------------------
bool
ThreadedQueue::isCreateThread() {
    #if ORYOL_HAS_THREADS
        return std::this_thread::get_id() == this->createThreadId;
    #else
        return true;
    #endif
}

//------------------------------------------------------------------------------
bool
ThreadedQueue::isWorkerThread() {
    #if ORYOL_HAS_THREADS
    return std::this_thread::get_id() == this->workThreadId;
    #else
    return true;
    #endif
}

//------------------------------------------------------------------------------
void
ThreadedQueue::moveWriteToTransferQueue() {
    o_assert(this->isCreateThread());
    // if the transfer queue is empty, we can do a very fast complete move
    #if ORYOL_HAS_THREADS
        this->transferQueueLock.lock();
    #endif
    if (this->transferQueue.Empty()) {
        this->transferQueue = std::move(this->writeQueue);
    }
    else {
        // otherwise move messages one by one
        while (!this->writeQueue.Empty()) {
            this->transferQueue.Enqueue(std::move(this->writeQueue.Dequeue()));
        }
    }
    #if ORYOL_HAS_THREADS
        this->transferQueueLock.unlock();
    #endif
}

//------------------------------------------------------------------------------
void
ThreadedQueue::moveTransferToReadQueue() {
    o_assert(this->isWorkerThread());
    o_assert(this->readQueue.Empty());
    #if ORYOL_HAS_THREADS
        this->transferQueueLock.lock();
    #endif
    this->readQueue = std::move(this->transferQueue);
    #if ORYOL_HAS_THREADS
        this->transferQueueLock.unlock();
    #endif
}

//------------------------------------------------------------------------------
#if ORYOL_HAS_THREADS
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
        if (0 != self->tickDuration) {
            // wait with timeout
            self->wakeup.wait_for(lock, std::chrono::milliseconds(self->tickDuration));
        }
        else {
            // wait infinitely for messages
            self->wakeup.wait(lock);
        }
        self->moveTransferToReadQueue();
        lock.unlock();
        
        // now process the messages, this happens without locking
        while (!self->readQueue.Empty()) {
            self->onMessage(std::move(self->readQueue.Dequeue()));
        }
        self->onTick();
    }
    
    // notify subclass that we're about to leave the thread
    self->onThreadLeave();
}
#endif

//------------------------------------------------------------------------------
/**
 The default implementation of onThreadEnter() will call 
 Core::Module::EnterThread() to setup any thread-locale data.
*/
void
ThreadedQueue::onThreadEnter() {
    Core::CoreFacade::EnterThread();
}

//------------------------------------------------------------------------------
/**
 The default implementation of onMessage will invoke the Put() method
 on the forwarding port with the message as argument.
*/
void
ThreadedQueue::onMessage(const Ptr<Message>& msg) {
    o_assert(this->forwardingPort.isValid());
    this->forwardingPort->Put(msg);
}

//------------------------------------------------------------------------------
/**
 The default implementation of onTick() will invoke the DoWork() method
 on the forwarding port.
*/
void
ThreadedQueue::onTick() {
    o_assert(this->forwardingPort.isValid());
    this->forwardingPort->DoWork();
}

//------------------------------------------------------------------------------
/**
 The default implementation of onThreadLeave() will call 
 Core::Module::LeaveThread() to discard any thread-locale data.
*/
void
ThreadedQueue::onThreadLeave() {
    Core::CoreFacade::LeaveThread();
}

} // namespace Messaging
} // namespace Oryol
