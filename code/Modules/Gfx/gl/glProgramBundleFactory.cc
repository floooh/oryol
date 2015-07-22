//------------------------------------------------------------------------------
//  glProgramBundleFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glProgramBundleFactory.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/resourcePools.h"
#include "Gfx/gl/gl_impl.h"
#include "Gfx/gl/glInfo.h"
#include "Gfx/gl/glTypes.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glProgramBundleFactory::glProgramBundleFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
glProgramBundleFactory::~glProgramBundleFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
glProgramBundleFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
glProgramBundleFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->pointers = gfxPointers();
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
glProgramBundleFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
glProgramBundleFactory::SetupResource(programBundle& progBundle) {
    o_assert_dbg(this->isValid);
    this->pointers.renderer->invalidateProgramState();

    #if (ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
    const ShaderLang::Code slang = ShaderLang::GLSL100;
    #elif ORYOL_OPENGL_CORE_PROFILE
    const ShaderLang::Code slang = ShaderLang::GLSL150;
    #else
    const ShaderLang::Code slang = ShaderLang::GLSL120;
    #endif
    const ProgramBundleSetup& setup = progBundle.Setup;

    // for each program in the bundle...
    const int32 numProgs = setup.NumPrograms();
    for (int32 progIndex = 0; progIndex < numProgs; progIndex++) {
        o_assert_dbg(setup.VertexShaderSource(progIndex, slang).IsValid());
        o_assert_dbg(setup.FragmentShaderSource(progIndex, slang).IsValid());
        
        // compile vertex shader
        const String& vsSource = setup.VertexShaderSource(progIndex, slang);
        GLuint glVertexShader = this->compileShader(ShaderType::VertexShader, vsSource.AsCStr(), vsSource.Length());
        o_assert_dbg(0 != glVertexShader);
        
        // compile fragment shader
        const String& fsSource = setup.FragmentShaderSource(progIndex, slang);
        GLuint glFragmentShader = this->compileShader(ShaderType::FragmentShader, fsSource.AsCStr(), fsSource.Length());
        o_assert_dbg(0 != glFragmentShader);
        
        // create GL program object and attach vertex/fragment shader
        GLuint glProg = ::glCreateProgram();
        ::glAttachShader(glProg, glVertexShader);
        ORYOL_GL_CHECK_ERROR();
        ::glAttachShader(glProg, glFragmentShader);
        ORYOL_GL_CHECK_ERROR();
        
        // bind vertex attribute locations
        /// @todo: would be good to optimize this to only bind
        /// attributes which exist in the shader (may be with more shader source generation)
        #if !ORYOL_GL_USE_GETATTRIBLOCATION
        o_assert_dbg(VertexAttr::NumVertexAttrs <= glInfo::Int(glInfo::MaxVertexAttribs));
        for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
            ::glBindAttribLocation(glProg, i, VertexAttr::ToString((VertexAttr::Code)i));
        }
        ORYOL_GL_CHECK_ERROR();
        #endif
        
        // link the program
        ::glLinkProgram(glProg);
        ORYOL_GL_CHECK_ERROR();
        
        // can discard shaders now if we compiled them ourselves
        ::glDeleteShader(glVertexShader);
        ::glDeleteShader(glFragmentShader);

        // linking successful?
        GLint linkStatus;
        ::glGetProgramiv(glProg, GL_LINK_STATUS, &linkStatus);
        #if ORYOL_DEBUG
        GLint logLength;
        ::glGetProgramiv(glProg, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar* logBuffer = (GLchar*) Memory::Alloc(logLength);
            ::glGetProgramInfoLog(glProg, logLength, &logLength, logBuffer);
            Log::Info("%s\n", logBuffer);
            Memory::Free(logBuffer);
        }
        #endif
        
        // if linking failed, stop the app
        if (!linkStatus) {
            o_warn("Failed to link program '%d' -> '%s'\n", progIndex, setup.Locator.Location().AsCStr());
            return ResourceState::Failed;
        }
        
        // linking succeeded, store GL program
        progBundle.addProgram(setup.Mask(progIndex), glProg);
        
        // resolve user uniform locations
        this->pointers.renderer->useProgram(glProg);
        const int32 numUniformBlocks = setup.NumUniformBlocks();
        for (int32 uniformBlockIndex = 0; uniformBlockIndex < numUniformBlocks; uniformBlockIndex++) {
            int32 samplerIndex = 0;
            int32 slotIndex = 0;
            const UniformLayout& layout = setup.UniformBlockLayout(uniformBlockIndex);
            const int32 numUniforms = layout.NumComponents();
            for (int uniformIndex = 0; uniformIndex < numUniforms; uniformIndex++) {
                const UniformLayout::Component& comp = layout.ComponentAt(uniformIndex);
                const GLint glLocation = ::glGetUniformLocation(glProg, comp.Name.AsCStr());
                progBundle.bindUniform(progIndex, uniformBlockIndex, slotIndex, glLocation);
                if (comp.Type == UniformType::Texture) {
                    progBundle.bindSamplerUniform(progIndex, uniformBlockIndex, slotIndex, glLocation, samplerIndex);
                    // set the sampler index in the shader program, this will never change
                    ::glUniform1i(glLocation, samplerIndex);
                    samplerIndex++;
                }
                slotIndex++;
            }
        }
        
        #if ORYOL_GL_USE_GETATTRIBLOCATION
        // resolve attrib locations
        for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
            GLint loc = ::glGetAttribLocation(glProg, VertexAttr::ToString((VertexAttr::Code)i));
            progBundle.bindAttribLocation(progIndex, (VertexAttr::Code)i, loc);
        }
        #endif
    }
    this->pointers.renderer->invalidateProgramState();
    
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glProgramBundleFactory::DestroyResource(programBundle& progBundle) {
    o_assert_dbg(this->isValid);
    this->pointers.renderer->invalidateProgramState();
    
    const int32 numProgs = progBundle.getNumPrograms();
    for (int32 progIndex = 0; progIndex < numProgs; progIndex++) {
        GLuint glProg = progBundle.getProgramAtIndex(progIndex);
        if (0 != glProg) {
            ::glDeleteProgram(glProg);
            ORYOL_GL_CHECK_ERROR();
        }
    }
    progBundle.Clear();
}

//------------------------------------------------------------------------------
GLuint
glProgramBundleFactory::compileShader(ShaderType::Code type, const char* sourceString, int sourceLen) const {
    o_assert_dbg(sourceString && (sourceLen > 0));
    
    GLuint glShader = glCreateShader(glTypes::asGLShaderType(type));
    o_assert_dbg(0 != glShader);
    ORYOL_GL_CHECK_ERROR();
    
    // attach source to shader object
    ::glShaderSource(glShader, 1, &sourceString, &sourceLen);
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
