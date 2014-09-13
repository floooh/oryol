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

} // namespace _priv
} // namespace Oryol