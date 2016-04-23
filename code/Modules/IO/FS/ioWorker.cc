//------------------------------------------------------------------------------
//  ioWorker.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ioWorker.h"
#include "IO/Core/schemeRegistry.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
ioWorker::start(const ioPointers& ptrs) {
    o_assert(!this->threadStartRequested);
    this->pointers = ptrs;
    #if ORYOL_HAS_THREADS
        this->sendThreadId = std::this_thread::get_id();
        this->thread = std::thread(threadFunc, this);
    #else
        this->onThreadEnter();
    #endif
    this->threadStartRequested = true;
}

//------------------------------------------------------------------------------
void
ioWorker::stop() {
    o_assert(this->threadStartRequested);
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
void
ioWorker::put(const Ptr<ioMsg>& msg) {
    o_assert(this->isSendThread());
    o_assert(this->threadStartRequested);
    o_assert(!this->threadStopped);
    this->writeQueue.Enqueue(msg);
}

//------------------------------------------------------------------------------
void
ioWorker::doWork() {
    // move messages to transfer queue and wake up thread if work needs to be done
    o_assert(this->isSendThread());
    o_assert(this->threadStartRequested);
    o_assert(!this->threadStopped);
    if (!this->writeQueue.Empty()) {
        this->moveWriteToTransferQueue();
    }
    o_assert_dbg(this->writeQueue.Empty());

    #if !ORYOL_HAS_THREADS
        // if platform has no threads, pump the message queue right
        // FIXME: we could do without all those queue transfers here!
        this->moveTransferToReadQueue();
        while (!this->readQueue.Empty()) {
            this->onMsg(std::move(this->readQueue.Dequeue()));
        }
    #endif
}

//------------------------------------------------------------------------------
#if ORYOL_HAS_THREADS
void
ioWorker::threadFunc(ioWorker* self) {
    self->workThreadId = std::this_thread::get_id();

    // the message processing loop waits for messages to arrive,
    // moves them from the transfer queue, processes them then goes back to sleep
    while (!self->threadStopRequested) {

        // wait for messages to arrive, and if so, transfer to read queue
        std::unique_lock<std::mutex> lock(self->wakeupMutex);
        self->wakeup.wait(lock);
        self->moveTransferToReadQueue();
        lock.unlock();
        
        // now process the messages, this happens without locking
        while (!self->readQueue.Empty()) {
            self->onMsg(std::move(self->readQueue.Dequeue()));
        }
    }
}
#endif

//------------------------------------------------------------------------------
bool
ioWorker::isSendThread() {
    #if ORYOL_HAS_THREADS
        return std::this_thread::get_id() == this->sendThreadId;
    #else
        return true;
    #endif
}

//------------------------------------------------------------------------------
bool
ioWorker::isWorkerThread() {
    #if ORYOL_HAS_THREADS
        return std::this_thread::get_id() == this->workThreadId;
    #else
        return true;
    #endif
}

//------------------------------------------------------------------------------
void
ioWorker::moveWriteToTransferQueue() {
    o_assert(this->isSendThread());
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
            this->transferQueue.Enqueue(this->writeQueue.Dequeue());
        }
    }
    #if ORYOL_HAS_THREADS
        // signal the worker thread if work is to be done
        if (!this->transferQueue.Empty()) {
            this->wakeup.notify_one();
        }
        this->transferQueueLock.unlock();
    #endif
}

//------------------------------------------------------------------------------
void
ioWorker::moveTransferToReadQueue() {
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
Ptr<FileSystem>
ioWorker::fileSystemForURL(const URL& url) {
    StringAtom scheme = url.Scheme();
    if (this->fileSystems.Contains(scheme)) {
        return this->fileSystems[scheme];
    }
    else {
        o_warn("ioLane::fileSystemForURL: no filesystem registered for URL scheme '%s'!\n", scheme.AsCStr());
        return Ptr<FileSystem>();
    }
}

//------------------------------------------------------------------------------
bool
ioWorker::checkCancelled(const Ptr<IORequest>& msg) {
    if (msg->Cancelled) {
        msg->Status = IOStatus::Cancelled;
        msg->Handled = true;
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
ioWorker::onMsg(const Ptr<ioMsg>& msg) {
    if (msg->IsA<IORequest>()) {
        // find filesystem and forward request
        Ptr<IORequest> ioReq = msg->DynamicCast<IORequest>();
        if (!this->checkCancelled(ioReq)) {
            Ptr<FileSystem> fs = this->fileSystemForURL(ioReq->Url);
            if (fs) {
                fs->onMsg(ioReq);
            }
        }
    }
    else if (msg->IsA<notifyWorkers>()) {
        // add, remove or replace a filesystem association
        const StringAtom& urlScheme = msg->DynamicCast<notifyWorkers>()->Scheme;
        if (msg->IsA<notifyFileSystemAdded>()) {
            o_assert(!this->fileSystems.Contains(urlScheme));
            Ptr<FileSystem> newFileSystem = this->pointers.schemeRegistry->CreateFileSystem(urlScheme);
            this->fileSystems.Add(urlScheme, newFileSystem);
        }
        else if (msg->IsA<notifyFileSystemRemoved>()) {
            o_assert(this->fileSystems.Contains(urlScheme));
            this->fileSystems.Erase(urlScheme);
        }
        else if (msg->IsA<notifyFileSystemReplaced>()) {
            o_assert(this->fileSystems.Contains(urlScheme));
            Ptr<FileSystem> newFileSystem = this->pointers.schemeRegistry->CreateFileSystem(urlScheme);
            this->fileSystems[urlScheme] = newFileSystem;
        }
    }
    msg->Handled = true;
}

} // namespace _priv
} // namespace Oryol
