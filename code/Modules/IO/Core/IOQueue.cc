//------------------------------------------------------------------------------
//  IOQueue.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IOQueue.h"
#include "IO/IOFacade.h"

namespace Oryol {
namespace IO {

//------------------------------------------------------------------------------
bool
IOQueue::Empty() const {
    return this->ioRequests.Empty();
}

//------------------------------------------------------------------------------
void
IOQueue::Add(const URL& url, int32 ioLane, SuccessFunc onSuccess, FailFunc onFail) {
    o_assert(onSuccess);

    // use global fail callback if no local provided
    if (!onFail) {
        onFail = this->OnFail;
    }
    auto req = IOFacade::Instance()->LoadFile(url, 0);
    this->ioRequests.AddBack(item{ req, onSuccess, onFail });
}

//------------------------------------------------------------------------------
void
IOQueue::Update() {
    for (int32 i = this->ioRequests.Size() - 1; i >= 0; --i) {
        const auto& cur = this->ioRequests[i];
        const auto& req = cur.ioRequest;
        if (req->Handled()) {
            // io request has been handled
            if (IOStatus::OK == req->GetStatus()) {
                // io request was successful
                cur.successFunc(req->GetStream());
            }
            else {
                // io request failed
                if (cur.failFunc) {
                    cur.failFunc(req->GetURL(), req->GetStatus());
                }
                else {
                    // no fail handler was set, error out
                    o_error("IOQueue::Update(): failed to load file '%s'\n", req->GetURL().AsCStr());
                }
            }
            // remove the handled io request from the queue
            this->ioRequests.Erase(i);
        }
    }
}

} // namespace IO
} // namespace Oryol