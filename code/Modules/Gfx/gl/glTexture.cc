//------------------------------------------------------------------------------
//  glTexture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glTexture.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
glTexture::glTexture() :
glTarget(0),
glFramebuffer(0),
glDepthRenderbuffer(0),
updateFrameIndex(-1),
numSlots(1),
activeSlot(0) {
    this->glTextures.Fill(0);
}

//------------------------------------------------------------------------------
glTexture::~glTexture() {
    o_assert_dbg(0 == this->glTarget);
    o_assert_dbg(0 == this->glFramebuffer);
    o_assert_dbg(0 == this->glDepthRenderbuffer);
    #if ORYOL_DEBUG
    for (const auto& glTex : this->glTextures) {
        o_assert_dbg(0 == glTex);
    }
    #endif
}

//------------------------------------------------------------------------------
void
glTexture::Clear() {
    textureBase::Clear();
    this->glTarget = 0;
    this->glFramebuffer = 0;
    this->glDepthRenderbuffer = 0;
    this->updateFrameIndex = -1;
    this->numSlots = 1;
    this->activeSlot = 0;
    this->glTextures.Fill(0);
}

} // namespace _priv
} // namespace Oryol
