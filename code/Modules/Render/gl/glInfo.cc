//------------------------------------------------------------------------------
//  glInfo.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glInfo.h"
#include "Core/Assert.h"
#include "Core/Log.h"
#include "Core/String/StringBuilder.h"
#include "Render/gl/gl_impl.h"

namespace Oryol {
namespace Render {

using namespace Core;

//------------------------------------------------------------------------------
void
glInfo::PrintInfo() {
    glInfo::PrintString(GL_VERSION, "GL_VERSION", false);
    glInfo::PrintString(GL_SHADING_LANGUAGE_VERSION, "GL_SHADING_LANGUAGE_VERSION", false);
    glInfo::PrintString(GL_VENDOR, "GL_VENDOR", false);
    glInfo::PrintString(GL_RENDERER, "GL_RENDERER", false);
    #if !ORYOL_MACOS
    // on OSX, core profile is used, where getting the extensions string is an error
    glInfo::PrintString(GL_EXTENSIONS, "GL_EXTENSIONS", true);
    #endif
}

//------------------------------------------------------------------------------
void
glInfo::PrintString(GLenum glEnum, const char* name, bool replaceSpaceWithNewLine) {
    o_assert(name);
    const char* rawStr = (const char*) ::glGetString(glEnum);
    ORYOL_GL_CHECK_ERROR();
    if (rawStr) {
        String str(rawStr);
        if (replaceSpaceWithNewLine) {
            StringBuilder strBuilder;
            strBuilder.Append(" ");
            strBuilder.Append(str);
            strBuilder.SubstituteAll(" ", "\n");
            str = strBuilder.GetString();
        }
        Log::Info("%s: %s\n", name, str.AsCStr());
    }
    else {
        Log::Warn("::glGetString() returned nullptr!\n");
    }
}

//------------------------------------------------------------------------------
void
glInfo::PrintInt(GLenum glEnum, const char* name, int dim) {
    o_assert(name);
    o_assert_range(dim, 4);
    GLint value[4];
    ::glGetIntegerv(glEnum, &(value[0]));
    ORYOL_GL_CHECK_ERROR();
    if (1 == dim) Log::Info("%s: %d\n", name, value[0]);
    else if (2 == dim) Log::Info("%s: %d %d\n", name, value[0], value[1]);
    else if (3 == dim) Log::Info("%s: %d %d %d\n", name, value[0], value[1], value[2]);
    else if (4 == dim) Log::Info("%s: %d %d %d %d\n", name, value[0], value[1], value[2], value[3]);
}

} // namespace Render
} // namespace Oryol
