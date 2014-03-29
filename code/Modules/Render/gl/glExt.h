#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glExt
    @brief function wrapper for extension functions
    
    This class wraps some GL extension function calls which are tricky
    to resolve (e.g. GL_APPLE_vertex_array_object vs GL_ARB_vertex_array_object)
*/
#include "Render/gl/gl_decl.h"

namespace Oryol {
namespace Render {

class glExt {
public:
    /// extension enums
    enum Code {
        VertexArrayObject = 0,

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

    /// glGenVertexArrays
    static void GenVertexArrays(GLsizei n, GLuint* arrays);
    /// glDeleteVertexArrays
    static void DeleteVertexArrays(GLsizei n, const GLuint* arrays);
    /// glBindVertexArray
    static void BindVertexArray(GLuint array);

private:
    static bool extensions[NumExtensions];
    static bool isValid;
};

    
} // namespace Render
} // namespace Oryol
