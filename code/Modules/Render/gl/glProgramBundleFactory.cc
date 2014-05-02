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

    // for each program in the bundle...
    const ProgramBundleSetup& setup = progBundle.GetSetup();
    const int32 numProgs = setup.GetNumPrograms();
    for (int32 progIndex = 0; progIndex < numProgs; progIndex++) {
        
        // lookup or compile vertex shader
        Map<String,String> noDefines;
        GLuint glVertexShader = 0;
        bool deleteVertexShader = false;
        if (setup.GetVertexShaderSource(progIndex).IsValid()) {
            // compile the vertex shader from source
            glVertexShader = this->shdFactory->compileShader(ShaderType::VertexShader, setup.GetVertexShaderSource(progIndex), noDefines);
            deleteVertexShader = true;
        }
        else {
            // vertex shader is precompiled
            const shader* vertexShader = this->shdPool->Lookup(setup.GetVertexShader(progIndex));
            o_assert(nullptr != vertexShader);
            glVertexShader = vertexShader->glGetShader();
        }
        o_assert(0 != glVertexShader);
        
        // lookup or compile fragment shader
        GLuint glFragmentShader = 0;
        bool deleteFragmentShader = false;
        if (setup.GetFragmentShaderSource(progIndex).IsValid()) {
            // compile the fragment shader from source
            glFragmentShader = this->shdFactory->compileShader(ShaderType::FragmentShader, setup.GetFragmentShaderSource(progIndex), noDefines);
            deleteFragmentShader = true;
        }
        else {
            // fragment shader is precompiled
            const shader* fragmentShader = this->shdPool->Lookup(setup.GetFragmentShader(progIndex));
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
        for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
            ::glBindAttribLocation(glProg, i, VertexAttr::ToString((VertexAttr::Code)i));
        }
        ORYOL_GL_CHECK_ERROR();
        
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
            o_error("Failed to link program '%d' -> '%s'\n", progIndex, setup.GetLocator().Location().AsCStr());
            progBundle.setState(Resource::State::Failed);
            return;
        }
        
        // linking succeeded, store GL program
        progBundle.addProgram(setup.GetMask(progIndex), glProg);
        
        // resolve user uniform locations
        this->glStateWrapper->UseProgram(glProg);
        int32 samplerIndex = 0;
        const int32 numUniforms = setup.GetNumUniforms();
        for (int32 i = 0; i < numUniforms; i++) {
            const String& name = setup.GetUniformName(i);
            const int16 slotIndex = setup.GetUniformSlot(i);
            const GLint glLocation = ::glGetUniformLocation(glProg, name.AsCStr());
            progBundle.bindUniform(progIndex, slotIndex, glLocation);
            
            // special case for texture samplers
            if (setup.IsTextureUniform(i)) {
                progBundle.bindSamplerUniform(progIndex, slotIndex, glLocation, samplerIndex);
                
                // set the sampler index in the shader program, this will never change
                ::glUniform1i(glLocation, samplerIndex++);
            }
        }
    }
    
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