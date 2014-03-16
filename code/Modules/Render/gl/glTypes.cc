//------------------------------------------------------------------------------
//  glTypes.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Render/gl/gl_impl.h"
#include "glTypes.h"
#include "Core/Assert.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
GLenum
glTypes::AsGLPrimitiveType(PrimitiveType::Code c) {
    switch (c) {
        case PrimitiveType::Points:        return GL_POINTS;
        case PrimitiveType::LineStrip:     return GL_LINE_STRIP;
        case PrimitiveType::LineLoop:      return GL_LINE_LOOP;
        case PrimitiveType::Lines:         return GL_LINES;
        case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
        case PrimitiveType::TriangleFan:   return GL_TRIANGLE_FAN;
        case PrimitiveType::Triangles:     return GL_TRIANGLES;
        default:
            o_error("glTypes::AsGLPrimitiveType: invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::AsGLIndexType(IndexType::Code c) {
    switch (c) {
        case IndexType::Index16: return GL_UNSIGNED_SHORT;
        case IndexType::Index32: return GL_UNSIGNED_INT;
        default:
            o_error("glTypes::AsGLIndexType: invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::AsGLTexImageFormat(PixelFormat::Code c) {
    switch (c) {
        case PixelFormat::R8G8B8A8:
        case PixelFormat::R5G5B5A1:
        case PixelFormat::R4G4B4A4:
            return GL_RGBA;
            
        case PixelFormat::R8G8B8:
        case PixelFormat::R5G6B5:
            return GL_RGB;
            
        case PixelFormat::D16:
        case PixelFormat::D32:
            return GL_DEPTH_COMPONENT;
            
        case PixelFormat::D24S8:
            return GL_DEPTH_STENCIL;
            
        default:
            // FIXME: add missing values
            o_error("glTypes::AsGLTexImageFormat(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::AsGLTexImageType(PixelFormat::Code c) {
    switch (c) {
        case PixelFormat::R8G8B8A8:
        case PixelFormat::R8G8B8:
            return GL_UNSIGNED_BYTE;
            
        case PixelFormat::R5G5B5A1:
            return GL_UNSIGNED_SHORT_5_5_5_1;
            
        case PixelFormat::R5G6B5:
            return GL_UNSIGNED_SHORT_5_6_5;
            
        case PixelFormat::R4G4B4A4:
            return GL_UNSIGNED_SHORT_4_4_4_4;
        
        case PixelFormat::D16:
            return GL_UNSIGNED_SHORT;
            
        case PixelFormat::D32:
            return GL_UNSIGNED_INT;
            
        case PixelFormat::D24S8:
            return GL_UNSIGNED_INT_24_8;
            
        default:
            // FIXME: add missing values
            o_error("glTypes::AsGLTexImageType(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::AsGLUsage(Usage::Code c) {
    switch (c) {
        case Usage::Immutable:      return GL_STATIC_DRAW;
        case Usage::DynamicWrite:   return GL_DYNAMIC_DRAW;
        case Usage::DynamicStream:  return GL_STREAM_DRAW;
        default:
            o_error("glTypes::AsGLUsage(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::AsGLTextureWrapMode(TextureWrapMode::Code c) {
    switch (c) {
        case TextureWrapMode::ClampToEdge:      return GL_CLAMP_TO_EDGE;
        case TextureWrapMode::Repeat:           return GL_REPEAT;
        case TextureWrapMode::MirroredRepeat:   return GL_MIRRORED_REPEAT;
        default:
            o_error("glTypes::AsGLTextureWrapMode(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::AsGLTextureFilterMode(TextureFilterMode::Code c) {
    switch (c) {
        case TextureFilterMode::Nearest:                return GL_NEAREST;
        case TextureFilterMode::Linear:                 return GL_LINEAR;
        case TextureFilterMode::NearestMipmapNearest:   return GL_NEAREST_MIPMAP_NEAREST;
        case TextureFilterMode::NearestMipmapLinear:    return GL_NEAREST_MIPMAP_LINEAR;
        case TextureFilterMode::LinearMipmapNearest:    return GL_LINEAR_MIPMAP_NEAREST;
        case TextureFilterMode::LinearMipmapLinear:     return GL_LINEAR_MIPMAP_LINEAR;
        default:
            o_error("glTypes::AsGLTextureFilterMode(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::AsGLTextureTarget(TextureType::Code c) {
    switch (c) {
        case TextureType::Texture2D: return GL_TEXTURE_2D;
        case TextureType::Texture3D: return GL_TEXTURE_3D;
        case TextureType::TextureCube: return GL_TEXTURE_CUBE_MAP;
        default:
            o_error("glTypes::AsGLTextureTarget(): invalid param!\n");
            return 0;
    }
}

} // namespace Render
} // namespace Oryol