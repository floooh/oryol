//------------------------------------------------------------------------------
//  mtlTexture.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlTexture.h"
#include "mtl_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlTexture::mtlTexture() :
mtlTex(nil),
mtlSamplerState(nil),
mtlDepthTex(nil) {
    // empty
}

//------------------------------------------------------------------------------
mtlTexture::~mtlTexture() {
    o_assert_dbg(nil == this->mtlTex);
    o_assert_dbg(nil == this->mtlSamplerState);
    o_assert_dbg(nil == this->mtlDepthTex);
}

//------------------------------------------------------------------------------
void
mtlTexture::Clear() {
    textureBase::Clear();
    this->mtlTex = nil;
    this->mtlSamplerState = nil;
    this->mtlDepthTex = nil;
}

} // namespace _priv
} // namespace Oryol