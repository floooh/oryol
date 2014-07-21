//------------------------------------------------------------------------------
//  glProgramBundleFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glProgramBundleFactory.h"
#include "Render/Core/stateWrapper.h"
#include "Render/Core/shader.h"
#include "Render/Core/shaderPool.h"
#include "Render/Core/shaderFactory.h"
#include "Render/gl/gl_impl.h"
#include "Render/gl/glExt.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace Render {

using namespace Core;
using namespace Resource;

//------------------------------------------------------------------------------
glProgramBundleFactory::glProgramBundleFactory() :
glStateWrapper(0),
shdPool(0),
shdFactory(0),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
glProgramBundleFactory::~glProgramBundleFactory() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void
glProgramBundleFactory::Setup(stateWrapper* stWrapper, shaderPool* pool, shaderFactory* factory) {
    o_assert(!this->isValid);
    o_assert(nullptr != stWrapper);
    o_assert(nullptr != pool);
    o_assert(nullptr != factory);
    this->isValid = true;
    this->glStateWrapper = stWrapper;
    this->shdPool = pool;
    this->shdFactory = factory;
}

//------------------------------------------------------------------------------
void
glProgramBundleFactory::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
    this->glStateWrapper = nullptr;
    this->shdPool = nullptr;
    this->shdFactory = nullptr;
}

//------------------------------------------------------------------------------
bool
glProgramBundleFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
glProgramBundleFactory::SetupResource(programBundle& progBundle) {
    o_assert(this->isValid);
    o_assert(progBundle.GetState() == Resource::State::Setup);
    this->glStateWrapper->InvalidateProgramState();

    #if ORYOL_OPENGLES2
    const ShaderLang::Code slang = ShaderLang::GLSL100;
    #elif ORYOL_MACOS
    const ShaderLang::Code slang = ShaderLang::GLSL150;
    #else
    const ShaderLang::Code slang = ShaderLang::GLSL120;
    #endif

    // for each program in the bundle...
    const ProgramBundleSetup& setup = progBundle.GetSetup();
    const int32 numProgs = setup.NumPrograms();
    for (int32 progIndex = 0; progIndex < numProgs; progIndex++) {
        
        // lookup or compile vertex shader
        Map<String,String> noDefines;
        GLuint glVertexShader = 0;
        bool deleteVertexShader = false;
        if (setup.VertexShaderSource(progIndex, slang).IsValid()) {
            // compile the vertex shader from source
            glVertexShader = this->shdFactory->compileShader(ShaderType::VertexShader, setup.VertexShaderSource(progIndex, slang));
            deleteVertexShader = true;
        }
        else {
            // vertex shader is precompiled
            const shader* vertexShader = this->shdPool->Lookup(setup.VertexShader(progIndex));
            o_assert(nullptr != vertexShader);
            glVertexShader = vertexShader->glGetShader();
        }
        o_assert(0 != glVertexShader);
        
        // lookup or compile fragment shader
        GLuint glFragmentShader = 0;
        bool deleteFragmentShader = false;
        if (setup.FragmentShaderSource(progIndex, slang).IsValid()) {
            // compile the fragment shader from source
            glFragmentShader = this->shdFactory->compileShader(ShaderType::FragmentShader, setup.FragmentShaderSource(progIndex, slang));
            deleteFragmentShader = true;
        }
        else {
            // fragment shader is precompiled
            const shader* fragmentShader = this->shdPool->Lookup(setup.FragmentShader(progIndex));
            o_assert(nullptr != fragmentShader);
            glFragmentShader = fragmentShader->glGetShader();
        }
        o_assert(0 != glFragmentShader);
        
        // create GL program object and attach vertex/fragment shader
        GLuint glProg = ::glCreateProgram();
        ::glAttachShader(glProg, glVertexShader);
        ORYOL_GL_CHECK_ERROR();
        ::glAttachShader(glProg, glFragmentShader);
        ORYOL_GL_CHECK_ERROR();
        
        // bind vertex attribute locations
        /// @todo: would be good to optimize this to only bind
        /// attributes which exist in the shader (may be with more shader source generation)
        #if !ORYOL_USE_GLGETATTRIBLOCATION
        o_assert(VertexAttr::NumVertexAttrs <= glExt::GetMaxVertexAttribs());
        for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
            ::glBindAttribLocation(glProg, i, VertexAttr::ToString((VertexAttr::Code)i));
        }
        ORYOL_GL_CHECK_ERROR();
        #endif
        
        // link the program
        ::glLinkProgram(glProg);
        ORYOL_GL_CHECK_ERROR();
        
        // can discard shaders now if we compiled them ourselves
        if (deleteVertexShader) {
            glDeleteShader(glVertexShader);
        }
        if (deleteFragmentShader) {
            glDeleteShader(glFragmentShader);
        }
        
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
            o_error("Failed to link program '%d' -> '%s'\n", progIndex, setup.Locator.Location().AsCStr());
            progBundle.setState(Resource::State::Failed);
            return;
        }
        
        // linking succeeded, store GL program
        progBundle.addProgram(setup.Mask(progIndex), glProg);
        
        // resolve user uniform locations
        this->glStateWrapper->UseProgram(glProg);
        int32 samplerIndex = 0;
        const int32 numUniforms = setup.NumUniforms();
        for (int32 i = 0; i < numUniforms; i++) {
            const String& name = setup.UniformName(i);
            const int16 slotIndex = setup.UniformSlot(i);
            const GLint glLocation = ::glGetUniformLocation(glProg, name.AsCStr());
            progBundle.bindUniform(progIndex, slotIndex, glLocation);
            
            // special case for texture samplers
            if (setup.IsTextureUniform(i)) {
                progBundle.bindSamplerUniform(progIndex, slotIndex, glLocation, samplerIndex);
                
                // set the sampler index in the shader program, this will never change
                ::glUniform1i(glLocation, samplerIndex++);
            }
        }
        
        #if ORYOL_USE_GLGETATTRIBLOCATION
        // resolve attrib locations
        for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
            GLint loc = ::glGetAttribLocation(glProg, VertexAttr::ToString((VertexAttr::Code)i));
            progBundle.bindAttribLocation(progIndex, (VertexAttr::Code)i, loc);
        }
        #endif
    }
    this->glStateWrapper->InvalidateProgramState();
    
    // at this point the whole programBundle object has been successfully setup
    progBundle.setState(Resource::State::Valid);
}

//------------------------------------------------------------------------------
void
glProgramBundleFactory::DestroyResource(programBundle& progBundle) {
    o_assert(this->isValid);
    this->glStateWrapper->InvalidateProgramState();
    
    const int32 numProgs = progBundle.getNumPrograms();
    for (int32 progIndex = 0; progIndex < numProgs; progIndex++) {
        GLuint glProg = progBundle.getProgramAtIndex(progIndex);
        if (0 != glProg) {
            ::glDeleteProgram(glProg);
            ORYOL_GL_CHECK_ERROR();
        }
    }
    progBundle.clear();
    progBundle.setState(Resource::State::Setup);
}
    
} // namespace Render
} // namespace Oryol