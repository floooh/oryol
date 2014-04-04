//------------------------------------------------------------------------------
//  glTexture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glTexture.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
glTexture::glTexture() :
glTex(0),
glTarget(0),
glFrameBuffer(0),
glDepthTexture(0) {
    // empty
}

//------------------------------------------------------------------------------
glTexture::~glTexture() {
    o_assert(0 == this->glTex);
    o_assert(0 == this->glTarget);
    o_assert(0 == this->glFrameBuffer);
    o_assert(0 == this->glDepthTexture);
}

//------------------------------------------------------------------------------
void
glTexture::clear() {
    textureBase::clear();
    this->glTex = 0;
    this->glTarget = 0;
    this->glFrameBuffer = 0;
    this->glDepthTexture = 0;
}

//------------------------------------------------------------------------------
void
glTexture::glSetTexture(GLuint tex) {
    this->glTex = tex;
}

//------------------------------------------------------------------------------
void
glTexture::glSetTarget(GLenum tgt) {
    this->glTarget = tgt;
}

//------------------------------------------------------------------------------
void
glTexture::glSetFrameBuffer(GLuint fb) {
    this->glFrameBuffer = fb;
}

//------------------------------------------------------------------------------
void
glTexture::glSetDepthTexture(GLuint tex) {
    this->glDepthTexture = tex;
}

} // namespace Render
} // namespace Oryol