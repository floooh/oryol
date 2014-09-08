//------------------------------------------------------------------------------
//  IOQueue.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IOQueue.h"
#include "Core/CoreFacade.h"
#include "Core/RunLoop.h"
#include "IO/IOFacade.h"

namespace Oryol {
namespace IO {

using namespace Core;

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
    this->runLoopId = CoreFacade::Instance()->RunLoop()->Add([this]() { this->update(); });
}

//------------------------------------------------------------------------------
void
IOQueue::Stop() {
    o_assert(this->isStarted);
    this->isStarted = false;
    CoreFacade::Instance()->RunLoop()->Remove(this->runLoopId);
    this->ioRequests.Clear();
}

//------------------------------------------------------------------------------
bool
IOQueue::IsStarted() const {
    return this->isStarted;
}

//------------------------------------------------------------------------------
bool
IOQueue::Empty() const {
    return this->ioRequests.Empty();
}

//------------------------------------------------------------------------------
void
IOQueue::Add(const URL& url, SuccessFunc onSuccess, FailFunc onFail) {
    o_assert(onSuccess);

    // create IO request and push into IO facade
    Ptr<IOProtocol::Get> ioReq = IOProtocol::Get::Create();
    ioReq->SetURL(url);
    IOFacade::Instance()->Put(ioReq);
    
    // add to our queue if pending requests
    this->ioRequests.Add(item{ ioReq, onSuccess, onFail });
}

//------------------------------------------------------------------------------
/**
    This is called per frame from the thread-local run-loop.
*/
void
IOQueue::update() {
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
