//------------------------------------------------------------------------------
//  glTexture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glTexture.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
glTexture::glTexture() :
glTex(0),
glTarget(0),
glFramebuffer(0),
glDepthRenderbuffer(0) {
    // empty
}

//------------------------------------------------------------------------------
glTexture::~glTexture() {
    o_assert_dbg(0 == this->glTex);
    o_assert_dbg(0 == this->glTarget);
    o_assert_dbg(0 == this->glFramebuffer);
    o_assert_dbg(0 == this->glDepthRenderbuffer);
}

//------------------------------------------------------------------------------
void
glTexture::Clear() {
    textureBase::Clear();
    this->glTex = 0;
    this->glTarget = 0;
    this->glFramebuffer = 0;
    this->glDepthRenderbuffer = 0;
}

} // namespace _priv
} // namespace Oryol
