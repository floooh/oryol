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
textureBase::Clear() {
    this->textureAttrs = TextureAttrs();
    resourceBase::Clear();
}

} // namespace _priv
} // namespace Oryol