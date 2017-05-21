//------------------------------------------------------------------------------
//  glTypes.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "gl_impl.h"
#include "Core/Assertion.h"
#include "glTypes.h"
#include "glCaps.h"

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
        case PixelFormat::R10G10B10A2:
            return GL_RGBA;
            
        case PixelFormat::RGB8:
        case PixelFormat::R5G6B5:
            return GL_RGB;

        case PixelFormat::L8:
        case PixelFormat::R32F:
        case PixelFormat::R16F:
            if (glCaps::IsFlavour(glCaps::GLES2)) {
                return GL_LUMINANCE;
            }
            else
            {
                return GL_RED;
            }
            
        case PixelFormat::DEPTH:
            return GL_DEPTH_COMPONENT;
            
        case PixelFormat::DEPTHSTENCIL:
            return GL_DEPTH_STENCIL;

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
            o_error("glTypes::asGLTexImageFormat(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::asGLTexImageInternalFormat(PixelFormat::Code c) {
    #if ORYOL_OPENGLES2
        return glTypes::asGLTexImageFormat(c);
    #else
        if (glCaps::IsFlavour(glCaps::GLES2)) {
            return glTypes::asGLTexImageFormat(c);
        }
        else
        {
            switch (c) {
                case PixelFormat::RGBA8:
                    return GL_RGBA8;
                case PixelFormat::R5G5B5A1:
                    return GL_RGB5_A1;
                case PixelFormat::RGBA4:
                    return GL_RGBA4;
                case PixelFormat::R10G10B10A2:
                    return GL_RGB10_A2;
                case PixelFormat::RGBA32F:
                    return GL_RGBA32F;
                case PixelFormat::RGBA16F:
                    return GL_RGBA16F;
                case PixelFormat::R32F:
                    return GL_R32F;
                case PixelFormat::R16F:
                    return GL_R16F;
                case PixelFormat::RGB8:
                    return GL_RGB8;
                case PixelFormat::L8:
                    return GL_R8;
                case PixelFormat::DEPTH:
                    return GL_DEPTH_COMPONENT16;
                case PixelFormat::DEPTHSTENCIL:
                    return GL_DEPTH24_STENCIL8;
                case PixelFormat::R5G6B5:
                    #if ORYOL_OPENGLES3
                        return GL_RGB565;
                    #else
                        return GL_RGB5;
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
                    o_error("glTypes::asGLTexImageFormat(): invalid param!\n");
                    return 0;
            }
        }
    #endif
}

//------------------------------------------------------------------------------
GLenum
glTypes::asGLTexImageType(PixelFormat::Code c) {
    switch (c) {
        case PixelFormat::RGBA32F:
        case PixelFormat::R32F:
            return GL_FLOAT;
        
        case PixelFormat::RGBA16F:
        case PixelFormat::R16F:
            return GL_HALF_FLOAT;
            
        case PixelFormat::RGBA8:
        case PixelFormat::RGB8:
        case PixelFormat::L8:
            return GL_UNSIGNED_BYTE;

        #if !ORYOL_OPENGLES2
        case PixelFormat::R10G10B10A2:
            return GL_UNSIGNED_INT_2_10_10_10_REV;
        #endif

        case PixelFormat::R5G5B5A1:
            return GL_UNSIGNED_SHORT_5_5_5_1;
            
        case PixelFormat::R5G6B5:
            return GL_UNSIGNED_SHORT_5_6_5;
            
        case PixelFormat::RGBA4:
            return GL_UNSIGNED_SHORT_4_4_4_4;
            
        case PixelFormat::DEPTH:
            return GL_UNSIGNED_SHORT;
            
        case PixelFormat::DEPTHSTENCIL:
            return GL_UNSIGNED_INT_24_8;

        default:
            o_error("glTypes::asGLTexImageType(): invalid param!\n");
            return 0;
    }
}
    
//------------------------------------------------------------------------------
GLenum
glTypes::asGLDepthAttachmentFormat(PixelFormat::Code c) {
    o_assert_dbg(PixelFormat::IsValidTextureDepthFormat(c));
    switch (c) {
        case PixelFormat::DEPTH:
            return GL_DEPTH_COMPONENT16;
        case PixelFormat::DEPTHSTENCIL:
            return GL_DEPTH24_STENCIL8;
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
glTypes::asGLPrimitiveMode(PrimitiveType::Code c) {
    switch (c) {
        case PrimitiveType::Points:
            return GL_POINTS;
        case PrimitiveType::Lines:
        case PrimitiveType::LineStrip:
            return GL_LINES;
        case PrimitiveType::Triangles:
        case PrimitiveType::TriangleStrip:
            return GL_TRIANGLES;
        default:
            o_error("glTypes::asGLPrimitiveMode(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::asGLShaderStage(ShaderStage::Code c) {
    switch (c) {
        case ShaderStage::VS: return GL_VERTEX_SHADER;
        case ShaderStage::FS: return GL_FRAGMENT_SHADER;
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
        case TextureType::TextureCube:  return GL_TEXTURE_CUBE_MAP;
        #if !ORYOL_OPENGLES2
        case TextureType::Texture3D:    return GL_TEXTURE_3D;
        case TextureType::TextureArray: return GL_TEXTURE_2D_ARRAY;
        #endif
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
        case Usage::Dynamic:    return GL_DYNAMIC_DRAW;
        case Usage::Stream:     return GL_STREAM_DRAW;
        default:
            o_error("glTypes::asGLBufferUsage(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::asGLCubeFaceTarget(int faceIndex) {
    switch (faceIndex) {
        case 0: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        case 1: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        case 2: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        case 3: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        case 4: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
        default: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    }
}

} // namespace _priv
} // namespace Oryol
