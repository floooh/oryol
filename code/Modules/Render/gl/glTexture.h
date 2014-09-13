#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glTexture
    @ingroup _priv
    @brief GL implementation of texture class
*/
#include "Render/base/textureBase.h"
#include "Render/gl/gl_decl.h"

namespace Oryol {
namespace _priv {
    
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
    void glSetFramebuffer(GLuint fb);
    /// get GL framebuffer (if this is a render target texture)
    GLuint glGetFramebuffer() const;
    /// set GL depth render buffer object (if render target texture with depth buffer, and no depth textures supported)
    void glSetDepthRenderbuffer(GLuint drb);
    /// get GL depth render buffer object
    GLuint glGetDepthRenderbuffer() const;
    /// set GL depth texture (if render target texture with depth buffer, and depth textures supported)
    void glSetDepthTexture(GLuint dtx);
    /// get GL depth texture (if render target texture with depth buffer, and depth textures supported)
    GLuint glGetDepthTexture() const;
    
protected:
    GLuint glTex;
    GLenum glTarget;
    GLuint glFramebuffer;
    GLuint glDepthRenderbuffer;
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
glTexture::glGetFramebuffer() const {
    return this->glFramebuffer;
}

//------------------------------------------------------------------------------
inline GLuint
glTexture::glGetDepthRenderbuffer() const {
    return this->glDepthRenderbuffer;
}

//------------------------------------------------------------------------------
inline GLuint
glTexture::glGetDepthTexture() const {
    return this->glDepthTexture;
}

} // namespace _priv
} // namespace Oryol
