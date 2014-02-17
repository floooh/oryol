//------------------------------------------------------------------------------
//  ioRequestRouter.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ioRequestRouter.h"

namespace Oryol {
namespace IO {

using namespace Core;
using namespace Messaging;

//------------------------------------------------------------------------------
ioRequestRouter::ioRequestRouter(int32 numLanes_) :
numLanes(numLanes_) {

    // create ioLanes
    this->ioLanes.Reserve(this->numLanes);
    for (int32 i = 0; i < this->numLanes; i++) {
        Ptr<ioLane> newLane = ioLane::Create();
        newLane->StartThread();
        this->ioLanes.AddBack(newLane);
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
    Ptr<IOProtocol::notifyLanes> notifyMsg = msg.dynamicCast<IOProtocol::notifyLanes>();
    if (notifyMsg.isValid()) {
        for (const auto& lane : this->ioLanes) {
            lane->Put(msg);
        }
        return true;
    }
    else {
        Ptr<IOProtocol::Request> req = msg.dynamicCast<IOProtocol::Request>();
        if (req.isValid()) {
            const int32 laneIndex = req->GetLane() % this->numLanes;
            this->ioLanes[laneIndex]->Put(msg);
            return true;
        }
    }
    // fallthrough: unrecognized message
    Log::Warn("ioRequestRouter::Put(): unrecognized message received!\n");
    return false;
}

//------------------------------------------------------------------------------
void
ioRequestRouter::DoWork() {
    for (const auto& lane : this->ioLanes) {
        lane->DoWork();
    }
}

} // namespace IO
} // namespace Oryol
