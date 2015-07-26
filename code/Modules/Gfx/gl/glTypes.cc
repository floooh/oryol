//------------------------------------------------------------------------------
//  glTypes.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Gfx/gl/gl_impl.h"
#include "glTypes.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
GLenum
glTypes::asGLTexImageFormat(PixelFormat::Code c) {
    switch (c) {
        case PixelFormat::RGBA8:
        case PixelFormat::R5G5B5A1:
        case PixelFormat::RGBA4:
        case PixelFormat::RGBA32F:
        case PixelFormat::RGBA16F:
            return GL_RGBA;
            
        case PixelFormat::RGB8:
        case PixelFormat::R5G6B5:
            return GL_RGB;
            
        case PixelFormat::L8:
            #if ORYOL_OPENGLES2
                return GL_LUMINANCE;
            #else
                return GL_RED;
            #endif
            
        case PixelFormat::D16:
        case PixelFormat::D32:
            return GL_DEPTH_COMPONENT;
            
        case PixelFormat::D24S8:
            #if ORYOL_OPENGLES2 && !ORYOL_EMSCRIPTEN
            return GL_DEPTH_STENCIL_OES;
            #else
            return GL_DEPTH_STENCIL;
            #endif
            
        case PixelFormat::DXT1:
            return 0x83F1;      // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
        case PixelFormat::DXT3:
            return 0x83F2;      // GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
        case PixelFormat::DXT5:
            return 0x83F3;      // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
        case PixelFormat::PVRTC2_RGB:
            return 0x8C01;      // GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
        case PixelFormat::PVRTC4_RGB:
            return 0x8C00;      // GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
        case PixelFormat::PVRTC2_RGBA:
            return 0x8C03;      // GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
        case PixelFormat::PVRTC4_RGBA:
            return 0x8C02;      // GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
        case PixelFormat::ETC2_RGB8:
            return 0x9274;      // GL_COMPRESSED_RGB8_ETC2
        case PixelFormat::ETC2_SRGB8:
            return 0x9275;      // GL_COMPRESSED_SRGB8_ETC2

        default:
            // FIXME: add missing values
            o_error("glTypes::asGLTexImageFormat(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::asGLTexImageInternalFormat(PixelFormat::Code c) {
    #if (ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
    return glTypes::asGLTexImageFormat(c);
    #else
    switch (c) {
        case PixelFormat::RGBA8:
            return GL_RGBA8;
        case PixelFormat::R5G5B5A1:
            return GL_RGB5_A1;
        case PixelFormat::RGBA4:
            return GL_RGBA4;
        case PixelFormat::RGBA32F:
            return GL_RGBA32F;
        case PixelFormat::RGBA16F:
            return GL_RGBA16F;
        case PixelFormat::RGB8:
            return GL_RGB8;
        case PixelFormat::L8:
            return GL_R8;
        case PixelFormat::D16:
            return GL_DEPTH_COMPONENT16;
        case PixelFormat::D32:
            return GL_DEPTH_COMPONENT32F;
        case PixelFormat::D24S8:
            return GL_DEPTH24_STENCIL8;
        case PixelFormat::R5G6B5:
            return GL_RGB5;
        case PixelFormat::DXT1:
            return 0x83F1;      // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
        case PixelFormat::DXT3:
            return 0x83F2;      // GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
        case PixelFormat::DXT5:
            return 0x83F3;      // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
        case PixelFormat::PVRTC2_RGB:
            return 0x8C01;      // GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
        case PixelFormat::PVRTC4_RGB:
            return 0x8C00;      // GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
        case PixelFormat::PVRTC2_RGBA:
            return 0x8C03;      // GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
        case PixelFormat::PVRTC4_RGBA:
            return 0x8C02;      // GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
        case PixelFormat::ETC2_RGB8:
            return 0x9274;      // GL_COMPRESSED_RGB8_ETC2
        case PixelFormat::ETC2_SRGB8:
            return 0x9275;      // GL_COMPRESSED_SRGB8_ETC2
            
        default:
            // FIXME: add missing values
            o_error("glTypes::asGLTexImageFormat(): invalid param!\n");
            return 0;
    }
    #endif
}

//------------------------------------------------------------------------------
GLenum
glTypes::asGLTexImageType(PixelFormat::Code c) {
    switch (c) {
        case PixelFormat::RGBA32F:
            return GL_FLOAT;
        
        case PixelFormat::RGBA16F:
            #if ORYOL_OPENGLES2 && !ORYOL_EMSCRIPTEN
            return GL_HALF_FLOAT_OES;
            #else
            return GL_HALF_FLOAT;
            #endif
            
        case PixelFormat::RGBA8:
        case PixelFormat::RGB8:
        case PixelFormat::L8:
            return GL_UNSIGNED_BYTE;
            
        case PixelFormat::R5G5B5A1:
            return GL_UNSIGNED_SHORT_5_5_5_1;
            
        case PixelFormat::R5G6B5:
            return GL_UNSIGNED_SHORT_5_6_5;
            
        case PixelFormat::RGBA4:
            return GL_UNSIGNED_SHORT_4_4_4_4;
            
        case PixelFormat::D16:
            return GL_UNSIGNED_SHORT;
            
        case PixelFormat::D32:
            return GL_UNSIGNED_INT;
            
        case PixelFormat::D24S8:
            #if ORYOL_OPENGLES2 && !ORYOL_EMSCRIPTEN
            return GL_UNSIGNED_INT_24_8_OES;
            #else
            return GL_UNSIGNED_INT_24_8;
            #endif
            
        default:
            // FIXME: add missing values
            o_error("glTypes::asGLTexImageType(): invalid param!\n");
            return 0;
    }
}
    
//------------------------------------------------------------------------------
GLenum
glTypes::asGLRenderbufferFormat(PixelFormat::Code c) {
    // NOTE: all color buffers are actually created as texture attachments,
    // so currently this method is only used for depth formats, and
    // once depth textures are supported will probably go away
    switch (c) {
        case PixelFormat::RGBA8:
            #if ORYOL_OPENGLES2 && !ORYOL_EMSCRIPTEN
            return GL_RGBA8_OES;
            #else
            return GL_RGBA8;
            #endif
        case PixelFormat::RGB8:
            #if ORYOL_OPENGLES2 && !ORYOL_EMSCRIPTEN
            return GL_RGB8_OES;
            #else
            return GL_RGB8;
            #endif
        case PixelFormat::R5G5B5A1: 
            return GL_RGB5_A1;
        case PixelFormat::RGBA4:
            return GL_RGBA4;
        case PixelFormat::D16:      
            return GL_DEPTH_COMPONENT16;
        case PixelFormat::D24S8:    
            #if ORYOL_OPENGLES2 && !ORYOL_EMSCRIPTEN
            return GL_DEPTH24_STENCIL8_OES;
            #else
            return GL_DEPTH24_STENCIL8;
            #endif
        default:
            o_error("glTypes::asGLRenderbufferFormat(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::asGLIndexType(IndexType::Code c) {
    switch (c) {
        case IndexType::Index16:    return GL_UNSIGNED_SHORT;
        case IndexType::Index32:    return GL_UNSIGNED_INT;
        default:
            o_error("glTypes::asGLIndexType(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::asGLPrimitiveType(PrimitiveType::Code c) {
    switch (c) {
        case PrimitiveType::Points:         return GL_POINTS;
        case PrimitiveType::Lines:          return GL_LINES;
        case PrimitiveType::LineStrip:      return GL_LINE_STRIP;
        case PrimitiveType::Triangles:      return GL_TRIANGLES;
        case PrimitiveType::TriangleStrip:  return GL_TRIANGLE_STRIP;
        default:
            o_error("glTypes::asGLPrimitiveType(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::asGLShaderType(ShaderType::Code c) {
    switch (c) {
        case ShaderType::VertexShader:      return GL_VERTEX_SHADER;
        case ShaderType::FragmentShader:    return GL_FRAGMENT_SHADER;
        default:
            o_error("glTypes::asGLShaderType(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::asGLTexFilterMode(TextureFilterMode::Code c) {
    switch (c) {
        case TextureFilterMode::Nearest:                return GL_NEAREST;
        case TextureFilterMode::Linear:                 return GL_LINEAR;
        case TextureFilterMode::NearestMipmapNearest:   return GL_NEAREST_MIPMAP_NEAREST;
        case TextureFilterMode::NearestMipmapLinear:    return GL_NEAREST_MIPMAP_LINEAR;
        case TextureFilterMode::LinearMipmapNearest:    return GL_LINEAR_MIPMAP_NEAREST;
        case TextureFilterMode::LinearMipmapLinear:     return GL_LINEAR_MIPMAP_LINEAR;
        default:
            o_error("glTypes::asGLTexFilterMode(): invalid param!\n");
            return 0;
    };
};

//------------------------------------------------------------------------------
GLenum
glTypes::asGLTexWrapMode(TextureWrapMode::Code c) {
    switch (c) {
        case TextureWrapMode::ClampToEdge:      return GL_CLAMP_TO_EDGE;
        case TextureWrapMode::Repeat:           return GL_REPEAT;
        case TextureWrapMode::MirroredRepeat:   return GL_MIRRORED_REPEAT;
        default:
            o_error("glTypes::asGLTexWrapMode(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::asGLTextureTarget(TextureType::Code c) {
    switch (c) {
        case TextureType::Texture2D:    return GL_TEXTURE_2D;
        case TextureType::Texture3D:    return GL_TEXTURE_3D;
        case TextureType::TextureCube:  return GL_TEXTURE_CUBE_MAP;
        default:
            o_error("glTypes::asGLTextureTarget(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::asGLBufferUsage(Usage::Code c) {
    switch (c) {
        case Usage::Immutable:  return GL_STATIC_DRAW;
        case Usage::Static:     return GL_STATIC_DRAW;
        case Usage::Dynamic:    return GL_DYNAMIC_DRAW;
        case Usage::Stream:     return GL_STREAM_DRAW;
        default:
            o_error("glTypes::asGLBufferUsage(): invalid param!\n");
            return 0;
    }
}

} // namespace _priv
} // namespace Oryol
