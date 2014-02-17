#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Messaging::ThreadedQueue
    @brief a threaded message queue port
    
    A message Port which acts as a threaded message queue. Will create
    a thread and forward messages to it. The forwarding port is running
    on the thread.
    
    The ThreadedQueue port will try to minimize locking by using 3
    internal queues: the write queue, the transfer queue and the read queue.
    
    Calling Put() will put a message on the write queue without locking.
    When DoWork() is called (usually once per frame), the sender thread
    will lock the transfer queue, push all messages from the write queue 
    on it, and signal the work thread. The work thread will wake up, 
    lock the transfer queue, move all messages from the transfer queue 
    to the read queue, and unlock the transfer queue again. It will then
    process messages from the read queue without locking.  When the read queue
    is empty it will check the transfer queue for more messages, and if this
    is empty, go to sleep.
*/
#include "Messaging/Port.h"
#include "Core/Containers/Queue.h"
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Oryol {
namespace Messaging {
    
class ThreadedQueue : public Port {
    OryolClassPoolAllocDecl(ThreadedQueue);
public:
    /// default constructor (must setup forwardingPort in subclass!)
    ThreadedQueue();
    /// constructor with forwarding port
    ThreadedQueue(const Core::Ptr<Port>& forwardingPort);
    /// destructor
    virtual ~ThreadedQueue();
    
    /// start the handler thread, this cannot happen in the constructor
    virtual void StartThread();
    /// stop the handler thread, this cannot happen in the destructor
    virtual void StopThread();
    /// put a message into the port
    virtual bool Put(const Core::Ptr<Message>& msg) override;
    /// perform work, this will be invoked on downstream ports
    virtual void DoWork();

protected:
    /// test if we are on the creation-thread
    bool isCreateThread();
    /// test if we are on the worker-thread
    bool isWorkerThread();
    /// the thread entry function
    static void threadFunc(ThreadedQueue* self);
    /// called in thread on thread-entry
    virtual void onThreadEnter();
    /// called to forward one message
    virtual void onMessage(const Core::Ptr<Message>& msg);
    /// called in thread before thread is left
    virtual void onThreadLeave();
    /// move messages from the write queue to the transfer queue
    void moveWriteToTransferQueue();
    /// move messages from transfer queue to read queue
    void moveTransferToReadQueue();
    
    Core::Queue<Core::Ptr<Message>> writeQueue;     // written by sender thread
    Core::Queue<Core::Ptr<Message>> transferQueue;  // written by sender, read by worker thread (locked)
    Core::Queue<Core::Ptr<Message>> readQueue;      // read by worker thread
    Core::Ptr<Port> forwardingPort;                // runs in thread!

    std::thread::id createThreadId;
    std::thread::id workThreadId;
    std::thread thread;
    std::mutex transferQueueLock;
    std::mutex wakeupMutex;
    std::condition_variable wakeup;
    bool threadStarted;
    bool threadStopRequested;
    bool threadStopped;
};
    
} // namespace Messaging
} // namespace Oryol
