#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glTexture
    @ingroup _priv
    @brief GL implementation of texture class
*/
#include "Gfx/Resource/textureBase.h"
#include "Gfx/gl/gl_decl.h"

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

    /// GL texture object
    GLuint glTex;
    /// GL texture target
    GLenum glTarget;
    /// GL frame buffer object
    GLuint glFramebuffer;
    /// GL depth render buffer
    GLuint glDepthRenderbuffer;
    /// GL depth texture
    GLuint glDepthTexture;
};

} // namespace _priv
} // namespace Oryol
