//------------------------------------------------------------------------------
//  glDebugOutput.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Log.h"
#include "glDebugOutput.h"
#include "glExt.h"
#include "gl_impl.h"

namespace Oryol {
namespace Render {

bool glDebugOutput::isEnabled = false;

//------------------------------------------------------------------------------
#if defined(GL_ARB_debug_output)
#if ORYOL_WINDOWS
void __stdcall
#else
void
#endif
debugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
    const char* typeStr;
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR_ARB:                   typeStr = "ERROR"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:     typeStr = "DEPRECATED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:      typeStr = "UNDEFINED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_PORTABILITY_ARB:             typeStr = "PORTABILITY"; break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:             typeStr = "PERFORMANCE"; break;
        case GL_DEBUG_TYPE_OTHER_ARB:                   typeStr = "OTHER"; break;
        default:                                        typeStr = "<unknown>"; break;
    }
    const char* sevStr;
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_LOW_ARB:     sevStr = "LOW"; break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB:  sevStr = "MEDIUM"; break;
        case GL_DEBUG_SEVERITY_HIGH_ARB:    sevStr = "HIGH"; break;
        default:                            sevStr = "<unknown>"; break;
}
    Core::Log::Warn("##### OpenGL Debug Message:\n"
        "type: %s, id: %d, severity: %s\nmsg: %s\n",
        typeStr, id, sevStr, message);
}
#endif

//------------------------------------------------------------------------------
void
glDebugOutput::Enable(Severity s) {
    #if defined(GL_ARB_debug_output)
    if (glExt::HasExtension(glExt::DebugOutput)) {
        GLenum glSeverity = GL_DONT_CARE;
        switch (s) {
            case High:      glSeverity = GL_DEBUG_SEVERITY_HIGH_ARB; break;
            case Medium:    glSeverity = GL_DEBUG_SEVERITY_MEDIUM_ARB; break;
            case Low:       glSeverity = GL_DEBUG_SEVERITY_LOW_ARB; break;
        }
        ::glDebugMessageControl(GL_DONT_CARE,        // source
                                GL_DONT_CARE,        // type
                                glSeverity,            // severity
                                0,                   // count
                                nullptr,             // ids
                                GL_TRUE);            // enabled
        if (!isEnabled) {
            ::glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
            ::glDebugMessageCallback(debugOutputCallback, nullptr);
            isEnabled = true;
        }
    }
    #endif
}

//------------------------------------------------------------------------------
void
glDebugOutput::Disable() {
    #if defined(GL_ARB_debug_output)
    if (glExt::HasExtension(glExt::DebugOutput)) {
        ::glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        isEnabled = false;
    }
    #endif
}

} // namespace Render
} // namespace Oryol