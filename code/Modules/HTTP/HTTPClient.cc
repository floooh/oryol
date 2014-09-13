//------------------------------------------------------------------------------
//  HTTPClient.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "HTTPClient.h"

namespace Oryol {
namespace HTTP {
    
OryolClassImpl(HTTPClient);

//------------------------------------------------------------------------------
HTTPClient::HTTPClient() {
    // empty
}

//------------------------------------------------------------------------------
HTTPClient::~HTTPClient() {
    // empty
}

//------------------------------------------------------------------------------
bool
HTTPClient::Put(const Ptr<Message>& msg) {
    o_assert(msg->IsMemberOf(HTTPProtocol::GetProtocolId()));
    Ptr<HTTPProtocol::HTTPRequest> req = msg.dynamicCast<HTTPProtocol::HTTPRequest>();
    o_assert(req.isValid());
    this->loader.putRequest(req);
    return true;
}

//------------------------------------------------------------------------------
void
HTTPClient::DoWork() {
    this->loader.doWork();
}

} // namespace HTTP
} // namespace Oryol