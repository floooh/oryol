#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glTexture
    @brief GL implementation of texture class
*/
#include "Render/base/textureBase.h"
#include "Render/gl/gl_decl.h"

namespace Oryol {
namespace Render {
    
class glTexture : public textureBase {
public:
    /// constructor
    glTexture();
    /// destructor
    ~glTexture();
    
    /// clear the object
    void clear();
    
    /// set the GL texture object
    void glSetTexture(GLuint tex);
    /// get the GL texture object
    GLuint glGetTexture() const;
    /// set the GL texture target (GL_TEXTURE_2D, ...)
    void glSetTarget(GLenum tgt);
    /// get the GL texture target
    GLuint glGetTarget() const;
    /// set GL framebuffer (if this is a render target texture)
    void glSetFrameBuffer(GLuint fb);
    /// get GL framebuffer (if this is a render target texture)
    GLuint glGetFrameBuffer() const;
    /// set GL depth texture (if render target texture with depth buffer, and depth textures supported)
    void glSetDepthTexture(GLuint dtx);
    /// get GL depth texture (if render target texture with depth buffer, and depth textures supported)
    GLuint glGetDepthTexture() const;
    
protected:
    GLuint glTex;
    GLenum glTarget;
    GLuint glFrameBuffer;
    GLuint glDepthTexture;
};

//------------------------------------------------------------------------------
inline GLuint
glTexture::glGetTexture() const {
    return this->glTex;
}

//------------------------------------------------------------------------------
inline GLenum
glTexture::glGetTarget() const {
    return this->glTarget;
}

//------------------------------------------------------------------------------
inline GLuint
glTexture::glGetFrameBuffer() const {
    return this->glFrameBuffer;
}

//------------------------------------------------------------------------------
inline GLuint
glTexture::glGetDepthTexture() const {
    return this->glDepthTexture;
}

} // namespace Render
} // namespace Oryol