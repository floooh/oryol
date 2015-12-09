//------------------------------------------------------------------------------
//  glExt.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Gfx/gl/gl_impl.h"
#include "glExt.h"
#include "Core/Assertion.h"
#include "Core/String/StringBuilder.h"

namespace Oryol {
namespace _priv {

bool glExt::isValid = false;
bool glExt::extensions[NumExtensions] = { false };
    
//------------------------------------------------------------------------------
void
glExt::Setup() {
    o_assert(!isValid);
    isValid = true;

    for (int32 i = 0; i < NumExtensions; i++) {
        extensions[i] = false;
    }
    
    #if !ORYOL_OPENGL_CORE_PROFILE
    StringBuilder strBuilder((const char*)::glGetString(GL_EXTENSIONS));
    ORYOL_GL_CHECK_ERROR();
    #endif
    
    #if ORYOL_OPENGL_CORE_PROFILE
    extensions[TextureCompressionDXT] = true;
    extensions[InstancedArrays] = true;
    extensions[TextureFloat] = true;
    #endif
    
    #if !ORYOL_OPENGL_CORE_PROFILE
    extensions[TextureCompressionDXT] = strBuilder.Contains("_texture_compression_s3tc") ||
                                        strBuilder.Contains("_compressed_texture_s3tc") ||
                                        strBuilder.Contains("_texture_compression_dxt1");
    extensions[TextureCompressionPVRTC] = strBuilder.Contains("_texture_compression_pvrtc") ||
                                          strBuilder.Contains("_compressed_texture_pvrtc");
    extensions[TextureCompressionATC] = strBuilder.Contains("_compressed_ATC_texture") ||
                                        strBuilder.Contains("_compressed_texture_atc");
    extensions[TextureFloat] = strBuilder.Contains("_texture_float");
    extensions[InstancedArrays] = strBuilder.Contains("_instanced_arrays");
    extensions[DebugOutput] = strBuilder.Contains("_debug_output");
    #endif
    
    #if ORYOL_OPENGLES2
    extensions[TextureHalfFloat] = strBuilder.Contains("_texture_half_float");
    #else
    // half-float-formats are included in the desktop GL extensions
    extensions[TextureHalfFloat] = extensions[TextureFloat];
    #endif

    #if ORYOL_OPENGLES3
    extensions[InstancedArrays] = true;
    #endif
    
    // put warnings to the console for extensions that we expect but are not provided
    if (!extensions[InstancedArrays]) {
        o_warn("glExt::Setup(): instanced_arrays extension not found!\n");
    }
}

//------------------------------------------------------------------------------
void
glExt::Discard() {
    o_assert(isValid);
    isValid = false;
}

//------------------------------------------------------------------------------
bool
glExt::IsValid() {
    return isValid;
}

//------------------------------------------------------------------------------
bool
glExt::IsTextureFormatSupported(PixelFormat::Code fmt) {
    if (PixelFormat::IsCompressedFormat(fmt)) {
        switch (fmt) {
            case PixelFormat::DXT1:
            case PixelFormat::DXT3:
            case PixelFormat::DXT5:
                return glExt::HasExtension(TextureCompressionDXT);
            case PixelFormat::PVRTC2_RGB:
            case PixelFormat::PVRTC4_RGB:
            case PixelFormat::PVRTC2_RGBA:
            case PixelFormat::PVRTC4_RGBA:
                return glExt::HasExtension(TextureCompressionPVRTC);
            #if ORYOL_OPENGLES3
            case PixelFormat::ETC2_RGB8:
            case PixelFormat::ETC2_SRGB8:
                return true;
            #endif
            default:
                return false;
        }
    }
    else {
        // non compressed format, always supported
        return true;
    }
}

//------------------------------------------------------------------------------
void
glExt::VertexAttribDivisor(GLuint index, GLuint divisor) {
    if (extensions[InstancedArrays]) {
        #if ORYOL_EMSCRIPTEN
        ::glVertexAttribDivisor(index, divisor);
        #elif ORYOL_IOS
        ::glVertexAttribDivisorEXT(index, divisor);
        #elif ORYOL_RASPBERRYPI
        o_error("glExt::VertexAttribDivisor() called!\n");
        #elif ORYOL_OPENGLES2
        ::glVertexAttribDivisorANGLE(index, divisor);
        #else
        ::glVertexAttribDivisor(index, divisor);
        #endif
    }
}

//------------------------------------------------------------------------------
void
glExt::DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount) {
    if (extensions[InstancedArrays]) {
        #if ORYOL_EMSCRIPTEN
        ::glDrawArraysInstanced(mode, first, count, primcount);
        #elif ORYOL_IOS
        ::glDrawArraysInstancedEXT(mode, first, count, primcount);
        #elif ORYOL_RASPBERRYPI
        o_error("glExt::DrawArraysInstanced() called!\n");
        #elif ORYOL_OPENGLES2
        ::glDrawArraysInstancedANGLE(mode, first, count, primcount);
        #else
        ::glDrawArraysInstanced(mode, first, count, primcount);
        #endif
    }
}

//------------------------------------------------------------------------------
void
glExt::DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount) {
    if (extensions[InstancedArrays]) {    
        #if ORYOL_EMSCRIPTEN
        ::glDrawElementsInstanced(mode, count, type, indices, primcount);
        #elif ORYOL_IOS
        ::glDrawElementsInstancedEXT(mode, count, type, indices, primcount);
        #elif ORYOL_RASPBERRYPI
        o_error("glExt::DrawElementsInstanced() called!\n");
        #elif ORYOL_OPENGLES2
        ::glDrawElementsInstancedANGLE(mode, count, type, indices, primcount);
        #else
        ::glDrawElementsInstanced(mode, count, type, indices, primcount);
        #endif
    }
}

} // namespace _priv
} // namespace Oryol

