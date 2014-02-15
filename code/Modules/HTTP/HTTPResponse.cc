//------------------------------------------------------------------------------
//  HTTPReponse.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "HTTPResponse.h"

namespace Oryol {
namespace HTTP {
    
OryolClassPoolAllocImpl(HTTPResponse);

using namespace IO;
using namespace Core;

//------------------------------------------------------------------------------
HTTPResponse::HTTPResponse() :
httpStatus(IOStatus::InvalidIOStatus) {
    // empty
}

//------------------------------------------------------------------------------
HTTPResponse::~HTTPResponse() {
    // empty
}

//------------------------------------------------------------------------------
void
HTTPResponse::SetStatus(IOStatus::Code status) {
    this->httpStatus = status;
}

//------------------------------------------------------------------------------
IOStatus::Code
HTTPResponse::GetStatus() const {
    return this->httpStatus;
}

//------------------------------------------------------------------------------
void
HTTPResponse::SetFields(const Map<String, String>& fields) {
    this->headerFields = fields;
}

//------------------------------------------------------------------------------
const Map<String, String>&
HTTPResponse::GetFields() const {
    return this->headerFields;
}

//------------------------------------------------------------------------------
void
HTTPResponse::SetBody(const Ptr<Stream>& stream) {
    this->body = stream;
}

//------------------------------------------------------------------------------
const Ptr<Stream>&
HTTPResponse::GetBody() const {
    return this->body;
}

} // namespace HTTP
} // namespace Oryol