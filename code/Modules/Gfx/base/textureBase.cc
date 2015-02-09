//------------------------------------------------------------------------------
//  textureBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "textureBase.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
textureBase::textureBase() {
    // empty
}

//------------------------------------------------------------------------------
void
textureBase::clear() {
    this->ioRequest.Invalidate();
    this->textureAttrs = TextureAttrs();
    resourceBase::clear();    
}

//------------------------------------------------------------------------------
void
textureBase::setIORequest(const Ptr<IOProtocol::Request>& req) {
    this->ioRequest = req;
}

} // namespace _priv
} // namespace Oryol