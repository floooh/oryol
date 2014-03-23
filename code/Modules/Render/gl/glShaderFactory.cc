//------------------------------------------------------------------------------
//  glShaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glShaderFactory.h"
#include "Render/Core/shader.h"
#include "Render/gl/glTypes.h"
#include "Render/gl/gl_impl.h"
#include "Core/String/StringBuilder.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace Render {

using namespace Core;
using namespace IO;
using namespace Resource;
    
//------------------------------------------------------------------------------
glShaderFactory::glShaderFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
glShaderFactory::~glShaderFactory() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void
glShaderFactory::Setup() {
    o_assert(!this->isValid);
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
glShaderFactory::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
glShaderFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
glShaderFactory::SetupResource(shader& shd) {
    o_assert(this->isValid);
    o_assert(shd.GetState() == State::Setup);
    
    // create a shader object
    const ShaderSetup& setup = shd.GetSetup();
    GLenum glShaderType = glTypes::AsGLShaderType(setup.GetType());
    const GLuint glShader = glCreateShader(glShaderType);
    o_assert(0 != glShader);
    ORYOL_GL_CHECK_ERROR();
    
    // estimate length of complete source string
    int32 srcLength = 0;
    for (const auto& define : setup.GetDefines()) {
        srcLength += 10 + define.Key().Length() + define.Value().Length();
    }
    srcLength += setup.GetSource().Length();
    
    // setup the shader source
    StringBuilder strBuilder;
    strBuilder.Reserve(2 * srcLength + 1024);
    #if ORYOL_OPENGL
        strBuilder.Append("#version 150\n");
        strBuilder.Append("#define ORYOL_OPENGL (1)\n");
        strBuilder.Append("#define lowp\n");
        strBuilder.Append("#define mediump\n");
        strBuilder.Append("#define highp\n");
        if (GL_VERTEX_SHADER == glShaderType) {
            strBuilder.Append("#define VERTEX_COMPONENT(type,name) in type name\n");
        }
        if (GL_FRAGMENT_SHADER == glShaderType) {
            strBuilder.Append("out vec4 FragmentColor;\n");
        }
    #elif ORYOL_OPENGLES2
        strBuilder.Append("#version 100\n");
        strBuilder.Append("#define ORYOL_OPENGLES2 (1)\n");
        if (GL_FRAGMENT_SHADER == glShaderType) {
            strBuilder.Append("precision mediump float;\n");
            strBuilder.Append("#define FragmentColor gl_FragColor\n");
        }
        if (GL_VERTEX_SHADER == glShaderType) {
            strBuilder.Append("#define VERTEX_COMPONENT(type,name) attribute type name");
        }
    #endif
    
    #if ORYOL_WINDOWS
        strBuilder.Append("#define ORYOL_WINDOWS (1)\n");
    #elif ORYOL_OSX
        strBuilder.Append("#define ORYOL_OSX (1)\n");
    #elif ORYOL_IOS
        strBuilder.Append("#define ORYOL_IOS (1)\n");
    #elif ORYOL_EMSCRIPTEN
        strBuilder.Append("#define ORYOL_EMSCRIPTEN (1)\n");
    #elif ORYOL_PNACL
        strBuilder.Append("#define ORYOL_PNACL (1)\n");
    #elif ORYOL_ANDROID
        strBuilder.Append("#define ORYOL_ANDROID (1)\n");
    #elif ORYOL_LINUX
        strBuilder.Append("#define ORYOL_LINUX (1)\n");
    #endif
    for (int32 i = 0; i < setup.GetDefines().Size(); i++) {
        const String& curDefineKey = setup.GetDefines().KeyAtIndex(i);
        const String& curDefineVal = setup.GetDefines().ValueAtIndex(i);
        strBuilder.Append("#define ");
        strBuilder.Append(curDefineKey);
        strBuilder.Append(' ');
        strBuilder.Append(curDefineVal);
        strBuilder.Append('\n');
    }
    strBuilder.Append(setup.GetSource());
    
    // attach source to shader object
    const GLchar* sourceString = strBuilder.AsCStr();
    const int sourceLength = strBuilder.Length();
    glShaderSource(glShader, 1, &sourceString, &sourceLength);
    ORYOL_GL_CHECK_ERROR();
    
    // compile the shader
    glCompileShader(glShader);
    ORYOL_GL_CHECK_ERROR();
    
    // compilation failed?
    GLint compileStatus = 0;
    GLint logLength = 0;
    glGetShaderiv(glShader, GL_COMPILE_STATUS, &compileStatus);
    glGetShaderiv(glShader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        
        // first get the shader source
        GLsizei shdSrcLen = 0;
        glGetShaderiv(glShader, GL_SHADER_SOURCE_LENGTH, &shdSrcLen);
        o_assert(srcLength > 0);
        GLchar* shdSrcBuf = (GLchar*) Memory::Alloc(shdSrcLen);
        glGetShaderSource(glShader, shdSrcLen, &shdSrcLen, shdSrcBuf);
        Log::Info("%s\n", shdSrcBuf);
        Memory::Free(shdSrcBuf);
        
        // now print the info log
        GLchar* shdLogBuf = (GLchar*) Memory::Alloc(logLength);
        glGetShaderInfoLog(glShader, logLength, &logLength, shdLogBuf);
        Log::Info("SHADER LOG: %s\n", shdLogBuf);
        Memory::Free(shdLogBuf);
    }
    
    // if compilation has failed, stop the program
    if (!compileStatus) {
        o_error("Failed to compile shader '%s'\n", setup.GetLocator().Location().AsCStr());
        shd.setState(State::Failed);
        return;
    }
    
    // all ok, shader has been successfully compiled
    shd.setShaderType(setup.GetType());
    shd.glSetShader(glShader);
    shd.setState(State::Valid);
}

//------------------------------------------------------------------------------
void
glShaderFactory::DestroyResource(shader& shd) {
    o_assert(this->isValid);

    const State::Code state = shd.GetState();
    o_assert(State::Valid == state);

    GLuint glShader = shd.glGetShader();
    o_assert(0 != glShader);
    glDeleteShader(glShader);
    
    shd.clear();
    shd.setState(State::Setup);
}

} // namespace Render
} // namespace Oryol
