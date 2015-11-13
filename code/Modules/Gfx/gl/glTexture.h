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
    void Clear();

    /// GL texture target
    GLenum glTarget;
    /// GL frame buffer object
    GLuint glFramebuffer;
    /// GL depth render buffer
    GLuint glDepthRenderbuffer;

    static const int32 MaxNumSlots = 2;
    int32 updateFrameIndex;
    uint8 numSlots;
    uint8 activeSlot;
    StaticArray<GLuint, MaxNumSlots> glTextures;
};

} // namespace _priv
} // namespace Oryol
