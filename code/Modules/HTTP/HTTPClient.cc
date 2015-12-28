//------------------------------------------------------------------------------
//  HTTPClient.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "HTTPClient.h"

namespace Oryol {
    
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
    Ptr<HTTPProtocol::HTTPRequest> req = msg->DynamicCast<HTTPProtocol::HTTPRequest>();
    o_assert(req.isValid());
    this->loader.doRequest(req);
    return true;
}

} // namespace Oryol
