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
glTypes::AsGLTexImageFormat(PixelFormat::Code c) {
    switch (c) {
        case PixelFormat::R8G8B8A8:
        case PixelFormat::R5G5B5A1:
        case PixelFormat::R4G4B4A4:
            return GL_RGBA;
            
        case PixelFormat::R8G8B8:
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
glTypes::AsGLRenderbufferFormat(PixelFormat::Code c) {
    switch (c) {
        case PixelFormat::R8G8B8A8: 
            #if ORYOL_OPENGLES2 && !ORYOL_EMSCRIPTEN
            return GL_RGBA8_OES;
            #else
            return GL_RGBA8;
            #endif
        case PixelFormat::R8G8B8:   
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
        case PixelFormat::R4G4B4A4: 
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

//------------------------------------------------------------------------------
GLenum
glTypes::AsGLTexImageType(PixelFormat::Code c) {
    switch (c) {
        case PixelFormat::R8G8B8A8:
        case PixelFormat::R8G8B8:
        case PixelFormat::L8:
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

} // namespace Render
} // namespace Oryol