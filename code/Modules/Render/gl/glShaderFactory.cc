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
    ORYOL_GL_CHECK_ERROR();
    
    Log::Info("glShaderFactory: compiling shader '%s'\n", shd.GetSetup().GetLocator().Location().AsCStr());
    
    // create a shader object
    const ShaderSetup& setup = shd.GetSetup();
    GLuint glShader = this->compileShader(setup.GetType(), setup.GetSource(), setup.GetDefines());
    
    // if compilation has failed, stop the program
    if (0 == glShader) {
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
    o_assert(State::Valid == shd.GetState());

    GLuint glShader = shd.glGetShader();
    o_assert(0 != glShader);
    glDeleteShader(glShader);
    ORYOL_GL_CHECK_ERROR();
    
    shd.clear();
    shd.setState(State::Setup);
}

//------------------------------------------------------------------------------
GLuint
glShaderFactory::compileShader(ShaderType::Code type, const String& src, const Map<String,String>& defines) const {
    o_assert(src.IsValid());
    
    GLenum glShaderType = glTypes::AsGLShaderType(type);
    GLuint glShader = glCreateShader(glShaderType);
    o_assert(0 != glShader);
    ORYOL_GL_CHECK_ERROR();
    
    // estimate length of complete source string
    int32 srcLength = 0;
    for (const auto& define : defines) {
        srcLength += 10 + define.Key().Length() + define.Value().Length();
    }
    srcLength += src.Length();
    
    // setup the shader source
    StringBuilder strBuilder;
    strBuilder.Reserve(2 * srcLength + 1024);
    #if ORYOL_OPENGLES2
        strBuilder.Append("#define ORYOL_OPENGLES2 (1)\n");
        if (GL_VERTEX_SHADER == glShaderType) {
            strBuilder.Append("#define VS_INPUT(type,name) attribute type name\n");
            strBuilder.Append("#define VS_OUTPUT(type,name) varying type name\n");
        }
        if (GL_FRAGMENT_SHADER == glShaderType) {
            strBuilder.Append("precision mediump float;\n");
            strBuilder.Append("#define FS_INPUT(type,name) varying type name\n");
            strBuilder.Append("#define TEXTURE2D(x,y) texture2D(x,y)\n");
            strBuilder.Append("#define TEXTURECUBE(x,y) textureCube(x,y)\n");
            strBuilder.Append("#define FragmentColor gl_FragColor\n");
        }
    #elif ORYOL_OSX
        // FIXME: this is actually the OpenGL 3.x Core Profile
        strBuilder.Append("#version 150\n");
        strBuilder.Append("#define ORYOL_OPENGL (1)\n");
        strBuilder.Append("#define lowp\n");
        strBuilder.Append("#define mediump\n");
        strBuilder.Append("#define highp\n");
        if (GL_VERTEX_SHADER == glShaderType) {
            strBuilder.Append("#define VS_INPUT(type,name) in type name\n");
            strBuilder.Append("#define VS_OUTPUT(type,name) out type name\n");
        }
        if (GL_FRAGMENT_SHADER == glShaderType) {
            strBuilder.Append("#define FS_INPUT(type,name) in type name\n");
            strBuilder.Append("#define TEXTURE2D(x,y) texture(x,y)\n");
            strBuilder.Append("#define TEXTURECUBE(x,y) texture(x,y)\n");
            strBuilder.Append("out vec4 FragmentColor;\n");
        }
    #else
        strBuilder.Append("#define ORYOL_OPENGL (1)\n");
        strBuilder.Append("#define lowp\n");
        strBuilder.Append("#define mediump\n");
        strBuilder.Append("#define highp\n");
        if (GL_VERTEX_SHADER == glShaderType) {
            strBuilder.Append("#define VS_INPUT(type,name) attribute type name\n");
            strBuilder.Append("#define VS_OUTPUT(type,name) varying type name\n");
        }
        if (GL_FRAGMENT_SHADER == glShaderType) {
            strBuilder.Append("#define FS_INPUT(type,name) varying type name\n");
            strBuilder.Append("#define TEXTURE2D(x,y) texture2D(x,y)\n");
            strBuilder.Append("#define TEXTURECUBE(x,y) textureCube(x,y)\n");
            strBuilder.Append("#define FragmentColor gl_FragColor\n");
        }
    #endif
    
    #if ORYOL_WINDOWS
        strBuilder.Append("#define ORYOL_WINDOWS (1)\n");
    #elif ORYOL_MACOS
        strBuilder.Append("#define ORYOL_MACOS (1)\n");
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
    for (int32 i = 0; i < defines.Size(); i++) {
        const String& curDefineKey = defines.KeyAtIndex(i);
        const String& curDefineVal = defines.ValueAtIndex(i);
        strBuilder.Append("#define ");
        strBuilder.Append(curDefineKey);
        strBuilder.Append(' ');
        strBuilder.Append(curDefineVal);
        strBuilder.Append('\n');
    }
    strBuilder.Append(src);
    
    // attach source to shader object
    const GLchar* sourceString = strBuilder.AsCStr();
    const int sourceLength = strBuilder.Length();
    glShaderSource(glShader, 1, &sourceString, &sourceLength);
    ORYOL_GL_CHECK_ERROR();
    
    // compile the shader
    ::glCompileShader(glShader);
    ORYOL_GL_CHECK_ERROR();
    
    // compilation failed?
    GLint compileStatus = 0;
    glGetShaderiv(glShader, GL_COMPILE_STATUS, &compileStatus);
    ORYOL_GL_CHECK_ERROR();
    
    #if ORYOL_DEBUG
        GLint logLength = 0;
        glGetShaderiv(glShader, GL_INFO_LOG_LENGTH, &logLength);
        ORYOL_GL_CHECK_ERROR();
        if (logLength > 0) {
            
            // first print the shader source
            Log::Info("SHADER SOURCE:\n%s\n\n", sourceString);
            
            // now print the info log
            GLchar* shdLogBuf = (GLchar*) Memory::Alloc(logLength);
            glGetShaderInfoLog(glShader, logLength, &logLength, shdLogBuf);
            ORYOL_GL_CHECK_ERROR();
            Log::Info("SHADER LOG: %s\n\n", shdLogBuf);
            Memory::Free(shdLogBuf);
        }
    #endif
    
    if (!compileStatus) {
        // compiling failed
        glDeleteShader(glShader);
        ORYOL_GL_CHECK_ERROR();
        glShader = 0;
    }
    return glShader;
}

} // namespace Render
} // namespace Oryol
