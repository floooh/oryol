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
    // FIXME: it should be possible to Stop the IOQueue from within
    // its success or fail func (currently this crashes because the
    // update loop is confused)

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
    Ptr<IOProtocol::Read> ioReq = IOProtocol::Read::Create();
    ioReq->Url = url;
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
        Ptr<IOProtocol::Read> ioReq = IOProtocol::Read::Create();
        ioReq->Url = url;
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
            if (IOStatus::OK == ioReq->Status) {
                // io request was successful
                curItem.successFunc(Result(ioReq->Url, std::move(ioReq->Data)));
            }
            else {
                // io request failed
                if (curItem.failFunc) {
                    curItem.failFunc(ioReq->Url, ioReq->Status);
                }
                else {
                    // no fail handler was set, just print a warning
                    o_warn("IOQueue:: failed to load file '%s' with '%s'\n",
                        ioReq->Url.AsCStr(), IOStatus::ToString(ioReq->Status));
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
                if (IOStatus::OK != ioReq->Status) {
                    anyFailed = true;
                    if (curItem.failFunc) {
                        curItem.failFunc(ioReq->Url, ioReq->Status);
                    }
                    else {
                        o_warn("IOQueue:: failed to load file '%s' with '%s'\n",
                            ioReq->Url.AsCStr(), IOStatus::ToString(ioReq->Status));
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
                Array<Result> result;
                result.Reserve(curItem.ioRequests.Size());
                for (const auto& ioReq : curItem.ioRequests) {
                    result.Add(ioReq->Url, std::move(ioReq->Data));
                }
                curItem.successFunc(std::move(result));
            }
            this->groupItems.Erase(i);
        }
        
    }
}

} // namespace Oryol
