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
textureBase::setIORequest(const Ptr<IOProtocol::Get>& req) {
    this->ioRequest = req;
}

//------------------------------------------------------------------------------
void
textureBase::setTextureAttrs(const TextureAttrs& attrs) {
    this->textureAttrs = attrs;
}

} // namespace _priv
} // namespace Oryol