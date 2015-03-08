#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glExt
    @ingroup _priv
    @brief function wrapper for extension functions
    
    This class wraps some GL extension function calls which are tricky
    to resolve (e.g. GL_APPLE_vertex_array_object vs GL_ARB_vertex_array_object)
*/
#include "Gfx/gl/gl_decl.h"
#include "Core/Assertion.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
namespace _priv {

class glExt {
public:
    /// extension enums
    enum Code {
        VertexArrayObject = 0,
        TextureCompressionDXT,
        TextureCompressionPVRTC,
        TextureCompressionATC,
        TextureFloat,
        TextureHalfFloat,
        InstancedArrays,
        DebugOutput,

        NumExtensions,
        InvalidExtension,
    };

    /// setup extensions
    static void Setup();
    /// discard extenstion wrapper object
    static void Discard();
    /// check if the extension wrapper object is valid
    static bool IsValid();
    /// test if an extension is supported
    static bool HasExtension(Code c);
    /// test if a texture format is supported
    static bool IsTextureFormatSupported(PixelFormat::Code fmt);

    /// glGenVertexArrays
    static void GenVertexArrays(GLsizei n, GLuint* arrays);
    /// glDeleteVertexArrays
    static void DeleteVertexArrays(GLsizei n, const GLuint* arrays);
    /// glBindVertexArray
    static void BindVertexArray(GLuint array);
    
    /// glVertexAttribDivisor
    static void VertexAttribDivisor(GLuint index, GLuint divisor);
    /// glDrawArraysInstanced
    static void DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount);
    /// glDrawElementsInstanced
    static void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount);

private:
    static bool extensions[NumExtensions];
    static bool isValid;
};

//------------------------------------------------------------------------------
inline bool
glExt::HasExtension(Code c) {
    o_assert_dbg(c < NumExtensions);
    return extensions[c];
}

} // namespace _priv
} // namespace Oryol
