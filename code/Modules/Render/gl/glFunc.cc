//------------------------------------------------------------------------------
//  glFunc.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Render/gl/gl_impl.h"
#include "glFunc.h"
#include "Core/Assert.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
void
glFunc::glGenVertexArrays(GLsizei n, GLuint* arrays) {
    #if ORYOL_OPENGLES2
        o_error("FIXME!\n");
    #elif ORYOL_OPENGL
        ::glGenVertexArrays(n, arrays);
    #else
    #error "Not an OpenGL platform!"
    #endif
}

//------------------------------------------------------------------------------
void
glFunc::glDeleteVertexArrays(GLsizei n, const GLuint* arrays) {
    #if ORYOL_OPENGLES2
        o_error("FIXME!\n");
    #elif ORYOL_OPENGL
        ::glDeleteVertexArrays(n, arrays);
    #else
    #error "Not an OpenGL platform!"
    #endif
}

//------------------------------------------------------------------------------
void
glFunc::glBindVertexArray(GLuint array) {
    #if ORYOL_OPENGLES2
        o_error("FIXME!\n");
    #elif ORYOL_OPENGL
        ::glBindVertexArray(array);
    #else
    #error "Not an OpenGL platform!"
    #endif
}

} // namespace Render
} // namespace Oryol