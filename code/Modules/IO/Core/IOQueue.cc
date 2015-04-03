//------------------------------------------------------------------------------
//  IOQueue.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IOQueue.h"
#include "Core/Core.h"
#include "Core/RunLoop.h"
#include "IO/IO.h"

namespace Oryol {

//------------------------------------------------------------------------------
IOQueue::IOQueue() :
isStarted(false),
runLoopId(RunLoop::InvalidId) {
    // empty
}

//------------------------------------------------------------------------------
IOQueue::~IOQueue() {
    o_assert_dbg(!this->isStarted);
}

//------------------------------------------------------------------------------
void
IOQueue::Start() {
    o_assert_dbg(!this->isStarted);
    this->isStarted = true;
    this->runLoopId = Core::PreRunLoop()->Add([this]() { this->update(); });
}

//------------------------------------------------------------------------------
void
IOQueue::Stop() {
    o_assert(this->isStarted);
    this->isStarted = false;
    Core::PreRunLoop()->Remove(this->runLoopId);
    this->items.Clear();
    this->groupItems.Clear();
}

//------------------------------------------------------------------------------
bool
IOQueue::IsStarted() const {
    return this->isStarted;
}

//------------------------------------------------------------------------------
bool
IOQueue::Empty() const {
    return this->items.Empty();
}

//------------------------------------------------------------------------------
void
IOQueue::Add(const URL& url, SuccessFunc onSuccess, FailFunc onFail) {
    o_assert_dbg(onSuccess);

    // create IO request and push into IO facade
    Ptr<IOProtocol::Request> ioReq = IOProtocol::Request::Create();
    ioReq->SetURL(url);
    IO::Put(ioReq);
    
    // add to our queue if pending requests
    this->items.Add(item{ ioReq, onSuccess, onFail });
}

//------------------------------------------------------------------------------
void
IOQueue::AddGroup(const Array<URL>& urls, GroupSuccessFunc onSuccess, FailFunc onFail) {
    o_assert_dbg(onSuccess);
    
    groupItem item;
    item.ioRequests.Reserve(urls.Size());
    for (const URL& url : urls) {
        Ptr<IOProtocol::Request> ioReq = IOProtocol::Request::Create();
        ioReq->SetURL(url);
        IO::Put(ioReq);
        item.ioRequests.Add(ioReq);
        item.successFunc = onSuccess;
        item.failFunc = onFail;
    }
    this->groupItems.Add(item);
}

//------------------------------------------------------------------------------
/**
    This is called per frame from the thread-local run-loop.
*/
void
IOQueue::update() {

    // check single items
    for (int i = this->items.Size() - 1; i >= 0; --i) {
        const item& curItem = this->items[i];
        const auto& ioReq = curItem.ioRequest;
        if (ioReq->Handled()) {
            // io request has been handled
            if (IOStatus::OK == ioReq->GetStatus()) {
                // io request was successful
                curItem.successFunc(ioReq->GetStream());
            }
            else {
                // io request failed
                if (curItem.failFunc) {
                    curItem.failFunc(ioReq->GetURL(), ioReq->GetStatus());
                }
                else {
                    // no fail handler was set, error out
                    o_error("IOQueue::update(): failed to load file '%s'\n", ioReq->GetURL().AsCStr());
                }
            }
            // remove the handled io request from the queue
            this->items.Erase(i);
        }
    }
    
    // check group items
    for (int i = this->groupItems.Size() - 1; i >= 0; --i) {
        const groupItem& curItem = this->groupItems[i];
        bool allHandled = true;
        bool anyFailed = false;
        for (const auto& ioReq : curItem.ioRequests) {
            if (ioReq->Handled()) {
                if (IOStatus::OK != ioReq->GetStatus()) {
                    anyFailed = true;
                    if (curItem.failFunc) {
                        curItem.failFunc(ioReq->GetURL(), ioReq->GetStatus());
                    }
                    else {
                        // no fail handler was set, throw fatal error
                        o_error("IOQueue::update(): failed to load file '%s'\n", ioReq->GetURL().AsCStr());
                    }
                }
            }
            else {
                allHandled = false;
                break;
            }
        }
        
        // if all request in this group have been handled, remove item, and
        // if all were successful, call the successFunc
        if (allHandled) {
            if (!anyFailed) {
                Array<Ptr<Stream>> result;
                result.Reserve(curItem.ioRequests.Size());
                for (const auto& ioReq : curItem.ioRequests) {
                    result.Add(ioReq->GetStream());
                }
                curItem.successFunc(result);
            }
        }
        
    }
}

} // namespace Oryol
