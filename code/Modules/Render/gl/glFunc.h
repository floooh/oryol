#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glFunc
    @brief function wrapper for some tricky GL functions
    
    This class wraps some GL extension function calls which are tricky
    to resolve (e.g. GL_APPLE_vertex_array_object vs GL_ARB_vertex_array_object)
*/
#include "Render/gl/gl_decl.h"

namespace Oryol {
namespace Render {

class glFunc {
public:
    /// glGenVertexArrays
    static void glGenVertexArrays(GLsizei n, GLuint* arrays);
    /// glDeleteVertexArrays
    static void glDeleteVertexArrays(GLsizei n, const GLuint* arrays);
    /// glBindVertexArray
    static void glBindVertexArray(GLuint array);
    
};

    
} // namespace Render
} // namespace Oryol