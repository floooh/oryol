//------------------------------------------------------------------------------
//  baseURLLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "baseURLLoader.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
bool
baseURLLoader::doRequest(const Ptr<HTTPProtocol::HTTPRequest>& httpReq) {

    // process one HTTP request, implement the actual downloading
    // in a subclass, we only handle the cancelled flag here
    const auto& ioReq = httpReq->IoRequest;
    if (ioReq.isValid()) {
        if (ioReq->Cancelled()) {
            ioReq->Status = IOStatus::Cancelled;
            ioReq->SetHandled();
            httpReq->SetCancelled();
            httpReq->SetHandled();
            return false;
        }
    }
    if (httpReq->Cancelled()) {
        httpReq->SetHandled();
        return false;
    }
    return true;
}

} // namespace _priv
} // namespace Oryol