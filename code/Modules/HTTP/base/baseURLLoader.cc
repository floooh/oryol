//------------------------------------------------------------------------------
//  baseURLLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "baseURLLoader.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
baseURLLoader::putRequest(const Ptr<HTTPProtocol::HTTPRequest>& req) {
    o_assert(req.isValid());
    this->requestQueue.Enqueue(req);
}

//------------------------------------------------------------------------------
void
baseURLLoader::doWork() {
    // override this method in platform-specific subclass
    o_error("baseURLLoader::doWork() called!\n");
}

//------------------------------------------------------------------------------
bool
baseURLLoader::handleCancelled(const Ptr<HTTPProtocol::HTTPRequest>& httpReq) {
    const auto& ioReq = httpReq->GetIoRequest();
    if (ioReq.isValid()) {
        if (ioReq->Cancelled()) {
            ioReq->SetStatus(IOStatus::Cancelled);
            ioReq->Handled();
            httpReq->SetCancelled();
            httpReq->Handled();
            return true;
        }
    }
    if (httpReq->Cancelled()) {
        httpReq->Handled();
        return true;
    }
    return false;
}

} // namespace _priv
} // namespace Oryol