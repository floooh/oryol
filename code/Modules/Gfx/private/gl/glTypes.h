#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glTypes
    @ingroup _priv
    @brief OpenGL type conversion helpers
*/
#include "Core/Types.h"
#include "Gfx/GfxTypes.h"
#include "Gfx/private/gl/gl_decl.h"

namespace Oryol {
namespace _priv {
    
class glTypes {
public:
    /// convert Oryol pixel format to glTexImage format
    static GLenum asGLTexImageFormat(PixelFormat::Code c);
    /// convert Oryol pixel format to glTexImage format, this is == TexImageFormat on GLES but may be different on desktop GL
    static GLenum asGLTexImageInternalFormat(PixelFormat::Code c);
    /// convert Oryol pixel format to glTexImage type
    static GLenum asGLTexImageType(PixelFormat::Code c);
    /// convert Oryol pixel format to depth attachment format
    static GLenum asGLDepthAttachmentFormat(PixelFormat::Code c);
    /// convert Oryol index type to GL index type
    static GLenum asGLIndexType(IndexType::Code c);
    /// convert Oryol primitive type to GL primitive type
    static GLenum asGLPrimitiveType(PrimitiveType::Code c);
    /// convert Oryol primitive type to GL primitive mode (points, lines, triangles)
    static GLenum asGLPrimitiveMode(PrimitiveType::Code c);
    /// convert Oryol shader type to GL shader type
    static GLenum asGLShaderStage(ShaderStage::Code c);
    /// convert Oryol texture filter to GL texture filter
    static GLenum asGLTexFilterMode(TextureFilterMode::Code c);
    /// convert Oryol texture wrap mode to GL texture wrap mode
    static GLenum asGLTexWrapMode(TextureWrapMode::Code c);
    /// convert Oryol texture type to GL texture target
    static GLenum asGLTextureTarget(TextureType::Code c);
    /// convert Oryol usage to GL buffer usage
    static GLenum asGLBufferUsage(Usage::Code c);
    /// convert cubemap face index to texture target
    static GLenum asGLCubeFaceTarget(int faceIndex);
};
    
} // namespace _priv
} // namespace Oryol
