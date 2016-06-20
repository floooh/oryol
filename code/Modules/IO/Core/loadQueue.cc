//------------------------------------------------------------------------------
//  loadQueue.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "loadQueue.h"
#include "Core/Core.h"
#include "Core/RunLoop.h"
#include "IO/IO.h"

namespace Oryol {

//------------------------------------------------------------------------------
void
loadQueue::add(const URL& url, successFunc onSuccess, failFunc onFail) {
    o_assert_dbg(onSuccess);
    Ptr<IORead> ioReq = IORead::Create();
    ioReq->Url = url;
    IO::Put(ioReq);
    this->items.Add(item{ ioReq, onSuccess, onFail });
}

//------------------------------------------------------------------------------
void
loadQueue::addGroup(const Array<URL>& urls, groupSuccessFunc onSuccess, failFunc onFail) {
    o_assert_dbg(onSuccess);
    
    groupItem item;
    item.ioRequests.Reserve(urls.Size());
    for (const URL& url : urls) {
        Ptr<IORead> ioReq = IORead::Create();
        ioReq->Url = url;
        IO::Put(ioReq);
        item.ioRequests.Add(ioReq);
        item.onSuccess = onSuccess;
        item.onFail = onFail;
    }
    this->groupItems.Add(item);
}

//------------------------------------------------------------------------------
int
loadQueue::numPending() const {
    return this->items.Size() + this->groupItems.Size();
}

//------------------------------------------------------------------------------
void
loadQueue::update() {

    // check single items
    for (int i = this->items.Size() - 1; i >= 0; --i) {
        const item& curItem = this->items[i];
        const auto& ioReq = curItem.ioRequest;
        if (ioReq->Handled) {
            // io request has been handled
            if (IOStatus::OK == ioReq->Status) {
                // io request was successful
                curItem.onSuccess(result(ioReq->Url, std::move(ioReq->Data)));
            }
            else {
                // io request failed
                if (curItem.onFail) {
                    curItem.onFail(ioReq->Url, ioReq->Status);
                }
                else {
                    // no fail handler was set, just print a warning
                    o_warn("loadQueue:: failed to load file '%s' with '%s'\n",
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
            if (ioReq->Handled) {
                if (IOStatus::OK != ioReq->Status) {
                    anyFailed = true;
                    if (curItem.onFail) {
                        curItem.onFail(ioReq->Url, ioReq->Status);
                    }
                    else {
                        o_warn("loadQueue:: failed to load file '%s' with '%s'\n",
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
        // if all were successful, call the success-callback
        if (allHandled) {
            if (!anyFailed) {
                Array<result> result;
                result.Reserve(curItem.ioRequests.Size());
                for (const auto& ioReq : curItem.ioRequests) {
                    result.Add(ioReq->Url, std::move(ioReq->Data));
                }
                curItem.onSuccess(std::move(result));
            }
            this->groupItems.Erase(i);
        }
    }
}

} // namespace Oryol

