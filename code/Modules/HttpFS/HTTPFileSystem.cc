//------------------------------------------------------------------------------
//  HTTPFileSystem.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "HTTPFileSystem.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
void
HTTPFileSystem::onMsg(const Ptr<IORequest>& ioReq) {
    Ptr<IORead> ioReadRequest = ioReq->DynamicCast<IORead>();
    if (ioReadRequest.isValid()) {
        this->loader.doRequest(ioReadRequest);
    }
}

} // namespace Oryol
