#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::ioWorker
    @ingroup IO
    @brief worker thread to forward IO requests to filesystem implementations
    
    An ioWorker is basically a message queue with a thread behind it. To
    remove granular locking it is pumped by the runloop. Once per
    runloop-frame, messages from the main thread will be moved to a
    'transfer queue', and the worker thread will be signaled. The 
    worker thread wakes up, moves the messages from the transfer queue
    to a read-queue, processes them and goes back to sleep.
*/
#include "Core/Config.h"
#include "Core/Containers/Queue.h"
#include "Core/Containers/Map.h"
#include "Core/String/StringAtom.h"
#include "IO/private/ioPointers.h"
#include "IO/private/ioRequests.h"
#include "IO/FileSystemBase.h"
#if ORYOL_HAS_THREADS
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#endif

namespace Oryol {
namespace _priv {

class ioWorker {
public:
    /// constructor
    ioWorker();
    /// setup and start the worker thread
    void start(const ioPointers& ptrs);
    /// stop the worker thread, wait for join
    void stop();
    /// put an io message into the internal message queue
    void put(const Ptr<ioMsg>& msg);
    /// do work on the main thread, this moves queued messages to transfer queue
    void doWork();

    /// lookup filesystem for URL
    Ptr<FileSystemBase> fileSystemForURL(const URL& url);
    /// check for and handle cancelled message
    bool checkCancelled(const Ptr<IORequest>& msg);
    /// called from thread to handle a generic message
    void onMsg(const Ptr<ioMsg>& msg);
    /// the thread worker func
    #if ORYOL_HAS_THREADS
    static void threadFunc(ioWorker* self);
    #endif
    /// test if we are on the send-thread
    bool isSendThread();
    /// test if we are on the worker-thread
    bool isWorkerThread();
    /// move messages from the write queue to the transfer queue
    void moveWriteToTransferQueue();
    /// move messages from transfer queue to read queue
    void moveTransferToReadQueue();

    ioPointers pointers;
    Map<StringAtom, Ptr<FileSystemBase>> fileSystems;

    Queue<Ptr<ioMsg>> writeQueue;     // written by sender thread
    Queue<Ptr<ioMsg>> transferQueue;  // written by sender, read by worker thread (locked)
    Queue<Ptr<ioMsg>> readQueue;      // read by worker thread

    #if ORYOL_HAS_THREADS
    std::thread::id sendThreadId;
    std::thread::id workThreadId;
    std::thread thread;
    std::mutex transferMutex;
    std::condition_variable transferCondVar;
    #endif
    #if ORYOL_HAS_ATOMIC
    std::atomic<bool> threadStopRequested;
    #else
    bool threadStopRequested;
    #endif
    bool threadStartRequested = false;
    bool threadStopped = false;
};


} // namespace _priv
} // namespace Oryol

    
