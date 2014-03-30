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
    
    for (int32 i = 0; i < NumExtensions; i++) {
        extensions[i] = false;
    }
    
    #if !ORYOL_OSX
    // on OSX we're using the Core Profile where getting the extensions string seems
    // to be an error
    Core::StringBuilder strBuilder((const char*)::glGetString(GL_EXTENSIONS));
    ORYOL_GL_CHECK_ERROR();
    extensions[VertexArrayObject] = strBuilder.Contains("_vertex_array_object");
    #endif
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
    #if ORYOL_OPENGLES2
        // FIXME: this works in emscripten for now
        ::glGenVertexArrays(n, arrays);
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
        // FIXME: this works in emscripten for now
        ::glDeleteVertexArrays(n, arrays);
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
        // FIXME: this works in emscripten for now
        ::glBindVertexArray(array);
    #elif ORYOL_OPENGL
        ::glBindVertexArray(array);
    #else
    #error "Not an OpenGL platform!"
    #endif
}

} // namespace Render
} // namespace Oryol
