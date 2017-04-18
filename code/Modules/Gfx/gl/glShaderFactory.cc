//------------------------------------------------------------------------------
//  glShaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glShaderFactory.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/resourcePools.h"
#include "Gfx/gl/gl_impl.h"
#include "Gfx/gl/glCaps.h"
#include "Gfx/gl/glTypes.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glShaderFactory::~glShaderFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
glShaderFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
glShaderFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->pointers = gfxPointers();
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
    this->pointers.renderer->invalidateShaderState();

    #if ORYOL_OPENGLES2
    const ShaderLang::Code slang = ShaderLang::GLSL100;
    #elif ORYOL_OPENGLES3
    const ShaderLang::Code slang = glCaps::IsFlavour(glCaps::GLES3) ? ShaderLang::GLSLES3 : ShaderLang::GLSL100;
    #elif ORYOL_OPENGL_CORE_PROFILE
    const ShaderLang::Code slang = ShaderLang::GLSL330;
    #else
    const ShaderLang::Code slang = ShaderLang::GLSL120;
    #endif
    const ShaderSetup& setup = shd.Setup;

    o_assert_dbg(setup.VertexShaderSource(slang).IsValid());
    o_assert_dbg(setup.FragmentShaderSource(slang).IsValid());

    // compile vertex shader
    const String& vsSource = setup.VertexShaderSource(slang);
    GLuint glVertexShader = this->compileShader(ShaderStage::VS, vsSource.AsCStr(), vsSource.Length());
    o_assert_dbg(0 != glVertexShader);
        
    // compile fragment shader
    const String& fsSource = setup.FragmentShaderSource(slang);
    GLuint glFragmentShader = this->compileShader(ShaderStage::FS, fsSource.AsCStr(), fsSource.Length());
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
    o_assert_dbg(VertexAttr::NumVertexAttrs <= glCaps::IntLimit(glCaps::MaxVertexAttribs));
    const VertexLayout& vsInputLayout = setup.InputLayout();
    for (int i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        VertexAttr::Code attr = (VertexAttr::Code)i;
        if (vsInputLayout.Contains(attr)) {
            ::glBindAttribLocation(glProg, i, VertexAttr::ToString(attr));
        }
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
    ORYOL_GL_CHECK_ERROR();

    // if linking failed, stop the app
    if (!linkStatus) {
        o_warn("Failed to link program '%s'\n", setup.Locator.Location().AsCStr());
        return ResourceState::Failed;
    }
        
    // linking succeeded, store GL program
    shd.glProgram = glProg;
    this->pointers.renderer->useProgram(glProg);

    // create uniform buffers?
    const int numUniformBlocks = setup.NumUniformBlocks();
    #if !ORYOL_OPENGLES2
    if (!glCaps::IsFlavour(glCaps::GLES2)) {
        for (int ubIndex = 0; ubIndex < numUniformBlocks; ubIndex++) {
            ShaderStage::Code ubBindStage = setup.UniformBlockBindStage(ubIndex);
            const StringAtom& ubName = setup.UniformBlockName(ubIndex);
            int ubBindSlot = setup.UniformBlockBindSlot(ubIndex);
            int ubArrayIndex = shd.uniformBlockArrayIndex(ubBindStage, ubBindSlot);
            shd.uniformBlockMappings[ubArrayIndex].uniformBufferIndex = ubIndex;
            ::glGenBuffers(1, &shd.uniformBlockMappings[ubArrayIndex].uniformBuffer);
            ORYOL_GL_CHECK_ERROR();
            ::glBindBufferBase(GL_UNIFORM_BUFFER, ubIndex, shd.uniformBlockMappings[ubArrayIndex].uniformBuffer);
            ORYOL_GL_CHECK_ERROR();
            this->strBuilder.Format(256, "%s_t", ubName.AsCStr());
            GLuint blockIndex = ::glGetUniformBlockIndex(glProg, this->strBuilder.AsCStr());
            ORYOL_GL_CHECK_ERROR();
            if (blockIndex == GL_INVALID_INDEX) {
                o_warn("Uniform block '%s' not found\n", ubName.AsCStr());
            }
            ::glUniformBlockBinding(glProg, blockIndex, ubIndex);
            ORYOL_GL_CHECK_ERROR();

            // allocate required uniform buffer space
            ::glGetActiveUniformBlockiv(glProg, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &shd.uniformBlockMappings[ubArrayIndex].dataSize);
            ORYOL_GL_CHECK_ERROR();
        }
    }
    #endif

    // resolve uniform locations
    for (int ubIndex = 0; ubIndex < numUniformBlocks; ubIndex++) {
        const UniformBlockLayout& layout = setup.UniformBlockLayout(ubIndex);
        ShaderStage::Code ubBindStage = setup.UniformBlockBindStage(ubIndex);
        const StringAtom& ubName = setup.UniformBlockName(ubIndex);
        int ubBindSlot = setup.UniformBlockBindSlot(ubIndex);
        const int numUniforms = layout.NumComponents();
        for (int uniformIndex = 0; uniformIndex < numUniforms; uniformIndex++) {
            const UniformBlockLayout::Component& comp = layout.ComponentAt(uniformIndex);
            this->strBuilder.Format(256, "%s.%s", ubName.AsCStr(), comp.Name.AsCStr());
            const GLint glUniformLocation = ::glGetUniformLocation(glProg, strBuilder.AsCStr());
            shd.bindUniform(ubBindStage, ubBindSlot, uniformIndex, glUniformLocation);
        }
    }
    ORYOL_GL_CHECK_ERROR();

    // resolve texture locations
    int glTextureLocation = 0;
    const int numTextureBlocks = setup.NumTextureBlocks();
    for (int tbIndex = 0; tbIndex < numTextureBlocks; tbIndex++) {
        const TextureBlockLayout& layout = setup.TextureBlockLayout(tbIndex);
        ShaderStage::Code tbBindStage = setup.TextureBlockBindStage(tbIndex);
        const int numTextures = layout.NumComponents();
        for (int texIndex = 0; texIndex < numTextures; texIndex++) {
            const TextureBlockLayout::Component& comp = layout.ComponentAt(texIndex);
            const GLint glUniformLocation = ::glGetUniformLocation(glProg, comp.Name.AsCStr());
            if (-1 != glUniformLocation) {
                shd.bindSampler(tbBindStage, texIndex, glTextureLocation);
                // set the sampler index in the shader program, this will never change
                ::glUniform1i(glUniformLocation, glTextureLocation);
                glTextureLocation++;
            }
            else {
                Log::Warn("Shader uniform '%s' not found, will be ignored!\n", comp.Name.AsCStr());
            }
        }
    }
    ORYOL_GL_CHECK_ERROR();

    #if ORYOL_GL_USE_GETATTRIBLOCATION
    // resolve attrib locations
    for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        GLint loc = ::glGetAttribLocation(glProg, VertexAttr::ToString((VertexAttr::Code)i));
        shd.bindAttribLocation((VertexAttr::Code)i, loc);
    }
    #endif

    this->pointers.renderer->invalidateShaderState();
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glShaderFactory::DestroyResource(shader& shd) {
    o_assert_dbg(this->isValid);
    this->pointers.renderer->invalidateShaderState();
    if (0 != shd.glProgram) {
        ::glDeleteProgram(shd.glProgram);
        ORYOL_GL_CHECK_ERROR();
    }
    #if !ORYOL_OPENGLES2
    if (!glCaps::IsFlavour(glCaps::GLES2)) {
        for (const auto& item : shd.uniformBlockMappings) {
            if (item.uniformBuffer) {
                ::glDeleteBuffers(1, &item.uniformBuffer);
            }
        }
    }
    #endif
    shd.Clear();
}

//------------------------------------------------------------------------------
GLuint
glShaderFactory::compileShader(ShaderStage::Code stage, const char* sourceString, int sourceLen) const {
    o_assert_dbg(sourceString && (sourceLen > 0));
    
    GLuint glShader = glCreateShader(glTypes::asGLShaderStage(stage));
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
