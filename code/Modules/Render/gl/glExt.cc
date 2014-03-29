//------------------------------------------------------------------------------
//  glExt.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Render/gl/gl_impl.h"
#include "glExt.h"
#include "Core/Assert.h"
#include "Core/String/StringBuilder.h"

namespace Oryol {
namespace Render {

bool glExt::isValid = false;
bool glExt::extensions[NumExtensions] = { false };
    
//------------------------------------------------------------------------------
void
glExt::Setup() {
    o_assert(!isValid);
    isValid = true;

    // initialize GLEW
    #if ORYOL_LINUX || ORYOL_WINDOWS
    glewInit();
    #endif

    Core::StringBuilder strBuilder((const char*)::glGetString(GL_EXTENSIONS));
    extensions[VertexArrayObject] = strBuilder.Contains("_vertex_array_object");
}

//------------------------------------------------------------------------------
void
glExt::GenVertexArrays(GLsizei n, GLuint* arrays) {
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
glExt::DeleteVertexArrays(GLsizei n, const GLuint* arrays) {
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
glExt::BindVertexArray(GLuint array) {
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
