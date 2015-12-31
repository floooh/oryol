//------------------------------------------------------------------------------
//  ioRequestRouter.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ioRequestRouter.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
ioRequestRouter::ioRequestRouter(int32 numLanes_, const ioPointers& ptrs) :
curLane(0),
numLanes(numLanes_) {

    // create ioLanes
    this->ioLanes.Reserve(this->numLanes);
    for (int32 i = 0; i < this->numLanes; i++) {
        Ptr<ioLane> newLane = ioLane::Create(ptrs);
        newLane->StartThread();
        this->ioLanes.Add(newLane);
    }
}

//------------------------------------------------------------------------------
ioRequestRouter::~ioRequestRouter() {
    for (const auto& lane : this->ioLanes) {
        lane->StopThread();
    }
    this->ioLanes.Clear();
}

//------------------------------------------------------------------------------
bool
ioRequestRouter::Put(const Ptr<Message>& msg) {
    // is it a notify message for all lanes?
    Ptr<IOProtocol::notifyLanes> notifyMsg = msg->DynamicCast<IOProtocol::notifyLanes>();
    if (notifyMsg.isValid()) {
        for (const auto& lane : this->ioLanes) {
            lane->Put(msg);
        }
        return true;
    }
    else {
        Ptr<IOProtocol::Request> req = msg->DynamicCast<IOProtocol::Request>();
        if (req.isValid()) {
            this->curLane = (this->curLane + 1) % this->numLanes;
            this->ioLanes[this->curLane]->Put(msg);
            return true;
        }
    }
    // fallthrough: unrecognized message
    o_warn("ioRequestRouter::Put(): unrecognized message received!\n");
    return false;
}

//------------------------------------------------------------------------------
void
ioRequestRouter::DoWork() {
    for (const auto& lane : this->ioLanes) {
        lane->DoWork();
    }
}

} // namespace _priv
} // namespace Oryol
