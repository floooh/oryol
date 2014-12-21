//------------------------------------------------------------------------------
//  glExt.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Gfx/gl/gl_impl.h"
#include "glExt.h"
#include "Core/Assert.h"
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
    extensions[VertexArrayObject] = true;
    extensions[TextureCompressionDXT] = true;
    extensions[InstancedArrays] = true;
    extensions[TextureFloat] = true;
    #endif
    
    #if !ORYOL_OPENGL_CORE_PROFILE
    extensions[VertexArrayObject] = strBuilder.Contains("_vertex_array_object");
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
    extensions[VertexArrayObject] = true;
    extensions[InstancedArrays] = true;
    #endif
    
    #if ORYOL_GL_DISABLE_VERTEXARRAYOBJECT
    extensions[VertexArrayObject] = false;
    #endif
    
    #if ORYOL_GL_USE_GETATTRIBLOCATION
    // If forced to use GetAttribLocation, VertexArrayObject must be disabled as
    // well since VertexArrayObject code path depends on BindAttribLocation
    extensions[VertexArrayObject] = false;
    #endif

    // put warnings to the console for extensions that we expect but are not provided
    if (!extensions[VertexArrayObject]) {
        Log::Warn("glExt::Setup(): vertex_array_object extension not found or disabled!\n");
    }
    if (!extensions[InstancedArrays]) {
        Log::Warn("glExt::Setup(): instanced_arrays extension not found!\n");
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
void
glExt::GenVertexArrays(GLsizei n, GLuint* arrays) {
    #if ORYOL_EMSCRIPTEN
        ::glGenVertexArrays(n, arrays);
    #elif ORYOL_OPENGLES2
        ::glGenVertexArraysOES(n, arrays);
    #else
        ::glGenVertexArrays(n, arrays);
    #endif
}

//------------------------------------------------------------------------------
void
glExt::DeleteVertexArrays(GLsizei n, const GLuint* arrays) {
    #if ORYOL_EMSCRIPTEN
        ::glDeleteVertexArrays(n, arrays);
    #elif ORYOL_OPENGLES2
        ::glDeleteVertexArraysOES(n, arrays);
    #else
        ::glDeleteVertexArrays(n, arrays);
    #endif
}

//------------------------------------------------------------------------------
void
glExt::BindVertexArray(GLuint array) {
    #if ORYOL_EMSCRIPTEN
        ::glBindVertexArray(array);
    #elif ORYOL_OPENGLES2
        ::glBindVertexArrayOES(array);
    #else
        ::glBindVertexArray(array);
    #endif
}

//------------------------------------------------------------------------------
void
glExt::VertexAttribDivisor(GLuint index, GLuint divisor) {
    if (extensions[InstancedArrays]) {
        #if ORYOL_EMSCRIPTEN
        ::glVertexAttribDivisor(index, divisor);
        #elif ORYOL_IOS
        ::glVertexAttribDivisorEXT(index, divisor);
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
        #elif ORYOL_OPENGLES2
        ::glDrawElementsInstancedANGLE(mode, count, type, indices, primcount);
        #else
        ::glDrawElementsInstanced(mode, count, type, indices, primcount);
        #endif
    }
}

} // namespace _priv
} // namespace Oryol
