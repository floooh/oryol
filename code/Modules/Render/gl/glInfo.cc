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
namespace _priv {

int32 glInfo::intValues[NumInfos] = { 0 };
bool glInfo::isValid = false;

//------------------------------------------------------------------------------
void
glInfo::Setup() {
    o_assert(!isValid);
    isValid = true;
    
    // setup int values
    ::glGetIntegerv(GL_MAX_TEXTURE_SIZE, &intValues[MaxTextureSize]);
    ::glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &intValues[MaxCubeMapTextureSize]);
    ::glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &intValues[MaxViewPortWidth]);
    ::glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &intValues[MaxVertexAttribs]);
    ::glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &intValues[MaxVertexUniformVectors]);
    ::glGetIntegerv(GL_MAX_VARYING_VECTORS, &intValues[MaxVaryingVectors]);
    ::glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &intValues[MaxCombinedTextureImageUnits]);
    ::glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &intValues[MaxVertexTextureImageUnits]);
    ::glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &intValues[MaxFragmentUniformVectors]);
    
    // print GL info
    printInfo();
}

//------------------------------------------------------------------------------
void
glInfo::Discard() {
    o_assert(isValid);
    isValid = false;
    Memory::Clear(intValues, sizeof(intValues));
}

//------------------------------------------------------------------------------
bool
glInfo::IsValid() {
    return isValid;
}

//------------------------------------------------------------------------------
int32
glInfo::Int(Code c) {
    o_assert_range_dbg(c, NumInfos);
    return intValues[c];
}

//------------------------------------------------------------------------------
void
glInfo::printInfo() {
    glInfo::printString(GL_VERSION, "GL_VERSION", false);
    glInfo::printString(GL_VENDOR, "GL_VENDOR", false);
    glInfo::printString(GL_RENDERER, "GL_RENDERER", false);
    glInfo::printString(GL_SHADING_LANGUAGE_VERSION, "GL_SHADING_LANGUAGE_VERSION", false);
    glInfo::printInt(GL_MAX_TEXTURE_SIZE, "GL_MAX_TEXTURE_SIZE", 1);
    glInfo::printInt(GL_MAX_CUBE_MAP_TEXTURE_SIZE, "GL_MAX_CUBE_MAP_TEXTURE_SIZE", 1);
    glInfo::printInt(GL_MAX_VIEWPORT_DIMS, "GL_MAX_VIEWPORT_DIMS", 2);
    glInfo::printInt(GL_MAX_VERTEX_ATTRIBS, "GL_MAX_VERTEX_ATTRIBS", 1);
    glInfo::printInt(GL_MAX_VERTEX_UNIFORM_VECTORS, "GL_MAX_VERTEX_UNIFORM_VECTORS", 1);
    glInfo::printInt(GL_MAX_VARYING_VECTORS, "GL_MAX_VARYING_VECTORS", 1);
    glInfo::printInt(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", 1);
    glInfo::printInt(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", 1);
    glInfo::printInt(GL_MAX_FRAGMENT_UNIFORM_VECTORS, "GL_MAX_FRAGMENT_UNIFORM_VECTORS", 1);
    #if !ORYOL_MACOS
    // on OSX, core profile is used, where getting the extensions string is an error
    glInfo::printString(GL_EXTENSIONS, "GL_EXTENSIONS", true);
    #endif
}

//------------------------------------------------------------------------------
void
glInfo::printString(GLenum glEnum, const char* name, bool replaceSpaceWithNewLine) {
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
glInfo::printInt(GLenum glEnum, const char* name, int dim) {
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

} // namespace _priv
} // namespace Oryol
