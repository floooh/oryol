//------------------------------------------------------------------------------
//  glTypes.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Render/gl/gl_impl.h"
#include "glTypes.h"
#include "Core/Assert.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
GLenum
glTypes::AsGLTexImageFormat(PixelFormat::Code c) {
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
            
        default:
            // FIXME: add missing values
            o_error("glTypes::AsGLTexImageFormat(): invalid param!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
GLenum
glTypes::AsGLTexImageInternalFormat(PixelFormat::Code c) {
    #if ORYOL_OPENGLES2
    return glTypes::AsGLTexImageFormat(c);
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
        default:
            // FIXME: add missing values
            o_error("glTypes::AsGLTexImageFormat(): invalid param!\n");
            return 0;
    }
    #endif
}

//------------------------------------------------------------------------------
GLenum
glTypes::AsGLTexImageType(PixelFormat::Code c) {
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
            o_error("glTypes::AsGLTexImageType(): invalid param!\n");
            return 0;
    }
}
    
//------------------------------------------------------------------------------
GLenum
glTypes::AsGLRenderbufferFormat(PixelFormat::Code c) {
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
#if !ORYOL_EMSCRIPTEN
        case PixelFormat::R5G6B5:   
            return GL_RGB565;
#endif
        case PixelFormat::R5G5B5A1: 
            return GL_RGB5_A1;
        case PixelFormat::RGBA4:
            return GL_RGBA4;
        case PixelFormat::D16:      
            return GL_DEPTH_COMPONENT16;
        case PixelFormat::D32:      
            #if ORYOL_OPENGLES2 && !ORYOL_EMSCRIPTEN
            return GL_DEPTH_COMPONENT32_OES;
            #else
            return GL_DEPTH_COMPONENT32;
            #endif
        case PixelFormat::D24S8:    
            #if ORYOL_OPENGLES2 && !ORYOL_EMSCRIPTEN
            return GL_DEPTH24_STENCIL8_OES;
            #else
            return GL_DEPTH24_STENCIL8;
            #endif
        default:
            o_error("glTypes::AsGLRenderbufferFormat(): invalid param!\n");
            return 0;
    }
}

} // namespace _priv
} // namespace Oryol
