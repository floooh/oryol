//------------------------------------------------------------------------------
//  glShaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glShaderFactory.h"
#include "Gfx/Resource/shader.h"
#include "Gfx/gl/glTypes.h"
#include "Gfx/gl/gl_impl.h"
#include "Core/String/StringBuilder.h"
#include "Core/Memory/Memory.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glShaderFactory::glShaderFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
glShaderFactory::~glShaderFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
glShaderFactory::Setup(renderer* /*rendr*/) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
glShaderFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
glShaderFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
glShaderFactory::SetupResource(shader& shd) {
    o_assert_dbg(this->isValid);
    ORYOL_GL_CHECK_ERROR();
    
    Log::Info("glShaderFactory: compiling shader '%s'\n", shd.Setup.Locator.Location().AsCStr());
    
    // create a shader object
    const ShaderSetup& setup = shd.Setup;
    
    #if (ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
    const ShaderLang::Code slang = ShaderLang::GLSL100;
    #elif ORYOL_MACOS
    const ShaderLang::Code slang = ShaderLang::GLSL150;
    #else
    const ShaderLang::Code slang = ShaderLang::GLSL120;
    #endif
    GLuint glShader = this->compileShader(setup.Type, setup.Source(slang));
    
    // if compilation has failed, stop the program
    if (0 == glShader) {
        o_warn("Failed to compile shader '%s'\n", setup.Locator.Location().AsCStr());
        return ResourceState::Failed;
    }
    
    // all ok, shader has been successfully compiled
    shd.shaderType = setup.Type;
    shd.glShd = glShader;
    
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glShaderFactory::DestroyResource(shader& shd) {
    o_assert_dbg(this->isValid);

    o_assert_dbg(0 != shd.glShd);
    ::glDeleteShader(shd.glShd);
    ORYOL_GL_CHECK_ERROR();
    
    shd.Clear();
}

//------------------------------------------------------------------------------
GLuint
glShaderFactory::compileShader(ShaderType::Code type, const String& src) const {
    o_assert_dbg(src.IsValid());
    
    GLuint glShader = glCreateShader(type);
    o_assert_dbg(0 != glShader);
    ORYOL_GL_CHECK_ERROR();
    
    // attach source to shader object
    const GLchar* sourceString = src.AsCStr();
    const int sourceLength = src.Length();
    ::glShaderSource(glShader, 1, &sourceString, &sourceLength);
    ORYOL_GL_CHECK_ERROR();
    
    // compile the shader
    ::glCompileShader(glShader);
    ORYOL_GL_CHECK_ERROR();
    
    // compilation failed?
    GLint compileStatus = 0;
    ::glGetShaderiv(glShader, GL_COMPILE_STATUS, &compileStatus);
    ORYOL_GL_CHECK_ERROR();
    
    #if ORYOL_DEBUG
        GLint logLength = 0;
        ::glGetShaderiv(glShader, GL_INFO_LOG_LENGTH, &logLength);
        ORYOL_GL_CHECK_ERROR();
        if (logLength > 0) {
            
            // first print the shader source
            Log::Info("SHADER SOURCE:\n%s\n\n", sourceString);
            
            // now print the info log
            GLchar* shdLogBuf = (GLchar*) Memory::Alloc(logLength);
            ::glGetShaderInfoLog(glShader, logLength, &logLength, shdLogBuf);
            ORYOL_GL_CHECK_ERROR();
            Log::Info("SHADER LOG: %s\n\n", shdLogBuf);
            Memory::Free(shdLogBuf);
        }
    #endif
    
    if (!compileStatus) {
        // compiling failed
        ::glDeleteShader(glShader);
        ORYOL_GL_CHECK_ERROR();
        glShader = 0;
    }
    return glShader;
}

} // namespace _priv
} // namespace Oryol

