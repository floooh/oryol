//------------------------------------------------------------------------------
//  osxURLLoader.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "osxURLLoader.h"

namespace Oryol {
namespace HTTP {

using namespace Core;
    
//------------------------------------------------------------------------------
void
osxURLLoader::doWork() {
    Ptr<HTTPProtocol::HTTPRequest> req;
    while (req = this->requestQueue.Dequeue(), req.isValid()) {
        Log::Info("FIXME: osxURLLoader process http request '%s'!\n", req->GetURL().AsCStr());
        req->SetHandled();
    }
}

} // namespace HTTP
} // namespace Oryol