//------------------------------------------------------------------------------
//  glProgramBundleFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glProgramBundleFactory.h"
#include "Render/Core/stateWrapper.h"
#include "Render/Core/shader.h"
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
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
glProgramBundleFactory::~glProgramBundleFactory() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void
glProgramBundleFactory::Setup(stateWrapper* stWrapper, shaderPool* pool) {
    o_assert(!this->isValid);
    o_assert(nullptr != stWrapper);
    o_assert(nullptr != pool);
    this->isValid = true;
    this->glStateWrapper = stWrapper;
    this->shdPool = pool;
}

//------------------------------------------------------------------------------
void
glProgramBundleFactory::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
    this->glStateWrapper = nullptr;
    this->shdPool = nullptr;
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

    // for each program in the bundle...
    const ProgramBundleSetup& setup = progBundle.GetSetup();
    const int32 numProgs = setup.GetNumPrograms();
    for (int32 progIndex = 0; progIndex < numProgs; progIndex++) {
        
        // lookup vertex and fragment shader objects
        const shader* vertexShader = this->shdPool->Lookup(setup.GetVertexShader(progIndex));
        o_assert(nullptr != vertexShader);
        const shader* fragmentShader = this->shdPool->Lookup(setup.GetFragmentShader(progIndex));
        o_assert(nullptr != fragmentShader);
        GLuint glVertexShader = vertexShader->glGetShader();
        o_assert(0 != glVertexShader);
        GLuint glFragmentShader = fragmentShader->glGetShader();
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
        
        // did it work?
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
        const int32 numUniforms = setup.GetNumUniforms();
        for (int32 i = 0; i < numUniforms; i++) {
            const String& name = setup.GetUniformName(i);
            const uint32 slotIndex = setup.GetUniformSlot(i);
            const GLint glLocation = ::glGetUniformLocation(glProg, name.AsCStr());
            progBundle.bindUniform(progIndex, slotIndex, glLocation);
        }
        
        // resolve standard uniform locations
        const int32 numStdUniforms = setup.GetNumStandardUniforms();
        for (int32 i = 0; i < numStdUniforms; i++) {
            const String& name = setup.GetStandardUniformName(i);
            StandardUniform::Code stdUniform = setup.GetStandardUniform(i);
            const GLint glLocation = ::glGetUniformLocation(glProg, name.AsCStr());
            progBundle.bindStandardUniform(progIndex, stdUniform, glLocation);
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
        }
    }
    progBundle.clear();
    progBundle.setState(Resource::State::Setup);
}
    
} // namespace Render
} // namespace Oryol