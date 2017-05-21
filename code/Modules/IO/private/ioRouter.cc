//------------------------------------------------------------------------------
//  ioRouter.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ioRouter.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
ioRouter::setup(const ioPointers& ptrs) {
    for (auto& worker : this->workers) {
        worker.start(ptrs);
    }
}

//------------------------------------------------------------------------------
void
ioRouter::discard() {
    for (auto& worker : this->workers) {
        worker.stop();
    }
}

//------------------------------------------------------------------------------
void
ioRouter::doWork() {
    for (auto& worker : this->workers) {
        worker.doWork();
    }
}

//------------------------------------------------------------------------------
void
ioRouter::put(const Ptr<ioMsg>& msg) {
    if (msg->IsA<notifyWorkers>()) {
        // notifyWorker messages must be distributed to all workers
        for (auto& worker : this->workers) {
            worker.put(msg);
        }
    }
    else {
        // for all other messages, use a round-robin dispatch
        this->curWorker = (this->curWorker + 1) % NumWorkers;
        this->workers[this->curWorker].put(msg);
    }
}

} // namespace _priv
} // namespace Oryol


