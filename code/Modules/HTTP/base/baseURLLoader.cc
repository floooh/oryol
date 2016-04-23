//------------------------------------------------------------------------------
//  baseURLLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "baseURLLoader.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
bool
baseURLLoader::doRequest(const Ptr<IORead>& ioReq) {
    // process one IO request, implement the actual downloading
    // in a subclass, we only handle the cancelled flag here
    if (ioReq->Cancelled) {
        ioReq->Status = IOStatus::Cancelled;
        ioReq->Handled = true;
        return false;
    }
    else {
        return true;
    }
}

} // namespace _priv
} // namespace Oryol