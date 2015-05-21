//------------------------------------------------------------------------------
//  glRenderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "Gfx/gl/gl_impl.h"
#include "glRenderer.h"
#include "Gfx/gl/glInfo.h"
#include "Gfx/gl/glExt.h"
#include "Gfx/gl/glTypes.h"
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Resource/texture.h"
#include "Gfx/Resource/programBundle.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/Resource/drawState.h"
#include "Gfx/Resource/meshPool.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Oryol {
namespace _priv {

GLenum glRenderer::mapCompareFunc[CompareFunc::NumCompareFuncs] = {
    GL_NEVER,
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS
};
    
GLenum glRenderer::mapStencilOp[StencilOp::NumStencilOperations] = {
    GL_KEEP,
    GL_ZERO,
    GL_REPLACE,
    GL_INCR,
    GL_DECR,
    GL_INVERT,
    GL_INCR_WRAP,
    GL_DECR_WRAP
};
    
GLenum glRenderer::mapBlendFactor[BlendFactor::NumBlendFactors] = {
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,
    GL_SRC_ALPHA_SATURATE,
    GL_CONSTANT_COLOR,
    GL_ONE_MINUS_CONSTANT_COLOR,
    GL_CONSTANT_ALPHA,
    GL_ONE_MINUS_CONSTANT_ALPHA,
};
    
GLenum glRenderer::mapBlendOp[BlendOperation::NumBlendOperations] = {
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
};
    
GLenum glRenderer::mapCullFace[Face::NumFaceCodes] = {
    GL_FRONT,
    GL_BACK,
    GL_FRONT_AND_BACK,
};

//------------------------------------------------------------------------------
glRenderer::glRenderer() :
valid(false),
dispMgr(nullptr),
mshPool(nullptr),
#if ORYOL_MACOS
globalVAO(0),
#endif
rtValid(false),
curRenderTarget(nullptr),
curDrawState(nullptr),
curMesh(nullptr),
curProgramBundle(nullptr),
scissorX(0),
scissorY(0),
scissorWidth(0),
scissorHeight(0),
blendColor(0.0f, 0.0f, 0.0f, 0.0f),
viewPortX(0),
viewPortY(0),
viewPortWidth(0),
viewPortHeight(0),
vertexBuffer(0),
indexBuffer(0),
vertexArrayObject(0),
program(0) {
    for (int32 i = 0; i < MaxTextureSamplers; i++) {
        this->samplers2D[i] = 0;
        this->samplersCube[i] = 0;
    }
}

//------------------------------------------------------------------------------
glRenderer::~glRenderer() {
    o_assert_dbg(!this->valid);
}

//------------------------------------------------------------------------------
void
glRenderer::setup(displayMgr* dispMgr_, meshPool* mshPool_) {
    o_assert_dbg(!this->valid);
    o_assert_dbg(dispMgr_);
    o_assert_dbg(mshPool_);
    
    this->valid = true;
    this->mshPool = mshPool_;
    this->dispMgr = dispMgr_;

    #if ORYOL_GL_USE_GETATTRIBLOCATION
    o_warn("glStateWrapper: ORYOL_GL_USE_GETATTRIBLOCATION is ON\n");
    #endif
    #if ORYOL_GL_DISABLE_VERTEXARRAYOBJECT
    o_warn("glStateWrapper: ORYOL_GL_DISABLE_VERTEXARRAYOBJECT is ON\n!");
    #endif
    
    // in case we are on a Core Profile, create a global Vertex Array Object
    #if ORYOL_MACOS
    ::glGenVertexArrays(1, &this->globalVAO);
    ::glBindVertexArray(this->globalVAO);
    #endif
    
    this->setupDepthStencilState();
    this->setupBlendState();
    this->setupRasterizerState();    
}

//------------------------------------------------------------------------------
void
glRenderer::discard() {
    o_assert_dbg(this->valid);
    
    this->invalidateMeshState();
    this->invalidateProgramState();
    this->invalidateTextureState();
    this->curRenderTarget = nullptr;
    this->curMesh = nullptr;
    this->curProgramBundle = nullptr;
    
    #if ORYOL_MACOS
    ::glDeleteVertexArrays(1, &this->globalVAO);
    this->globalVAO = 0;
    #endif

    this->mshPool = nullptr;
    this->dispMgr = nullptr;
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
glRenderer::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
glRenderer::resetStateCache() {
    o_assert_dbg(this->valid);

    this->setupDepthStencilState();
    this->setupBlendState();
    this->setupRasterizerState();
    this->invalidateMeshState();
    this->invalidateProgramState();
    this->invalidateTextureState();
}

//------------------------------------------------------------------------------
bool
glRenderer::supports(GfxFeature::Code feat) const {
    o_assert_dbg(this->valid);

    switch (feat) {
        case GfxFeature::TextureCompressionDXT:
            return glExt::HasExtension(glExt::TextureCompressionDXT);
        case GfxFeature::TextureCompressionPVRTC:
            return glExt::HasExtension(glExt::TextureCompressionPVRTC);
        case GfxFeature::TextureCompressionATC:
            return glExt::HasExtension(glExt::TextureCompressionATC);
        case GfxFeature::TextureCompressionETC2:
            #if ORYOL_OPENGLES3
            return true;
            #else
            return false;
            #endif
        case GfxFeature::TextureFloat:
            return glExt::HasExtension(glExt::TextureFloat);
        case GfxFeature::TextureHalfFloat:
            return glExt::HasExtension(glExt::TextureHalfFloat);
        case GfxFeature::Instancing:
            return glExt::HasExtension(glExt::InstancedArrays);
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
void
glRenderer::commitFrame() {
    o_assert_dbg(this->valid);    
    this->rtValid = false;
}

//------------------------------------------------------------------------------
void
glRenderer::applyViewPort(int32 x, int32 y, int32 width, int32 height) {
    o_assert_dbg(this->valid);

    if ((x != this->viewPortX) ||
        (y != this->viewPortY) ||
        (width != this->viewPortWidth) ||
        (height != this->viewPortHeight)) {
        
        this->viewPortX = x;
        this->viewPortY = y;
        this->viewPortWidth = width;
        this->viewPortHeight = height;
        ::glViewport(x, y, width, height);
    }
}

//------------------------------------------------------------------------------
void
glRenderer::applyRenderTarget(texture* rt) {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->dispMgr);
    
    // also update view port to cover full render target
    if (nullptr == rt) {
        this->rtAttrs = this->dispMgr->GetDisplayAttrs();
    }
    else {
        // FIXME: hmm, have a 'AsDisplayAttrs' util function somewhere?
        const TextureAttrs& attrs = rt->textureAttrs;
        this->rtAttrs.WindowWidth = attrs.Width;
        this->rtAttrs.WindowHeight = attrs.Height;
        this->rtAttrs.WindowPosX = 0;
        this->rtAttrs.WindowPosY = 0;
        this->rtAttrs.FramebufferWidth = attrs.Width;
        this->rtAttrs.FramebufferHeight = attrs.Height;
        this->rtAttrs.ColorPixelFormat = attrs.ColorFormat;
        this->rtAttrs.DepthPixelFormat = attrs.DepthFormat;
        this->rtAttrs.Samples = 1;
        this->rtAttrs.Windowed = false;
        this->rtAttrs.SwapInterval = 1;
    }
    
    // apply the frame buffer
    if (rt != this->curRenderTarget) {
        // default render target?
        if (nullptr == rt) {
            this->dispMgr->glBindDefaultFramebuffer();
        }
        else {
            ::glBindFramebuffer(GL_FRAMEBUFFER, rt->glFramebuffer);
            ORYOL_GL_CHECK_ERROR();
        }
    }
    this->curRenderTarget = rt;
    this->rtValid = true;
    
    // set viewport to cover whole screen
    this->applyViewPort(0, 0, this->rtAttrs.FramebufferWidth, this->rtAttrs.FramebufferHeight);
    
    // reset scissor test
    if (this->rasterizerState.ScissorTestEnabled) {
        this->rasterizerState.ScissorTestEnabled = false;
        ::glDisable(GL_SCISSOR_TEST);
    }
}

//------------------------------------------------------------------------------
void
glRenderer::applyScissorRect(int32 x, int32 y, int32 width, int32 height) {
    o_assert_dbg(this->valid);

    if ((x != this->scissorX) ||
        (y != this->scissorY) ||
        (width != this->scissorWidth) ||
        (height != this->scissorHeight)) {
        
        this->scissorX = x;
        this->scissorY = y;
        this->scissorWidth = width;
        this->scissorHeight = height;
        ::glScissor(x, y, width, height);
    }
}

//------------------------------------------------------------------------------
void
glRenderer::applyBlendColor(const glm::vec4& c) {
    o_assert_dbg(this->valid);

    if (c != this->blendColor) {
        this->blendColor = c;
        ::glBlendColor(c.x, c.y, c.z, c.w);
    }
}

//------------------------------------------------------------------------------
void
glRenderer::applyProgramBundle(programBundle* progBundle, uint32 mask) {
    o_assert_dbg(this->valid);

    progBundle->selectProgram(mask);
    GLuint glProg = progBundle->getProgram();
    o_assert_dbg(0 != glProg);
    this->useProgram(glProg);
}

//------------------------------------------------------------------------------
void
glRenderer::applyMesh(const mesh* msh, const programBundle* progBundle) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != msh);

    // bind the mesh
    // FIXME: record and compare against a 'current mesh pointer' whether
    // mesh state must be reapplied
    const uint8 vaoIndex = msh->getActiveVAOSlot();

    #if ORYOL_GL_USE_GETATTRIBLOCATION
        // FIXME: UNTESTED
        // This is the code path which uses glGetAttribLocation instead of
        // glBindAttribLocation, which must be used if GL_MAX_VERTEX_ATTRIBS is smaller
        // then VertexAttr::NumVertexAttrs. The only currently known platform
        // where this applies is the Raspberry Pi where GL_MAX_VERTEX_ATTRIBS==8
        o_assert_dbg(progBundle->getProgram() == this->curProgram);
        const GLuint ib = msh->glGetIndexBuffer();
        GLuint vb = 0;
        this->BindIndexBuffer(ib);
        int32 maxUsedAttrib = 0;
        for (int8 attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
            const glVertexAttr& attr = msh->glAttr(vaoIndex, attrIndex);
            const GLint glAttribIndex = progBundle->getAttribLocation((VertexAttr::Code)attrIndex);
            if (-1 != glAttribIndex) {
                o_assert_dbg(attr.vertexBuffer);
                if (attr.vertexBuffer != vb) {
                    vb = attr.vertexBuffer;
                    this->BindVertexBuffer(vb);
                }
                ::glEnableVertexAttribArray(glAttribIndex);
                ORYOL_GL_CHECK_ERROR();
                glExt::VertexAttribDivisor(glAttribIndex, attr.divisor);
                ORYOL_GL_CHECK_ERROR();
                ::glVertexAttribPointer(glAttribIndex, attr.size, attr.type, attr.normalized, attr.stride, (const GLvoid*) (GLintptr) attr.offset);
                ORYOL_GL_CHECK_ERROR();
                maxUsedAttrib++;
            }
        }
        int32 maxAttribs = glInfo::Int(glInfo::MaxVertexAttribs);
        if (VertexAttr::NumVertexAttrs < maxAttribs) {
            maxAttribs = VertexAttr::NumVertexAttrs;
        }
        for (int32 i = maxUsedAttrib; i < maxAttribs; i++) {
            ::glDisableVertexAttribArray(i);
        }
    #else
        if (glExt::HasExtension(glExt::VertexArrayObject)) {
            GLuint vao = msh->glVAOs[vaoIndex];
            o_assert_dbg(0 != vao);
            this->bindVertexArrayObject(vao);
        }
        else {
            GLuint vb = 0;
            const GLuint ib = msh->glIndexBuffer;
            this->bindIndexBuffer(ib);
            ORYOL_GL_CHECK_ERROR();
            for (uint8 attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
                const glVertexAttr& attr = msh->glAttrs[vaoIndex][attrIndex];
                if (attr.enabled) {
                    o_assert_dbg(attr.vertexBuffer);
                    if (attr.vertexBuffer != vb) {
                        vb = attr.vertexBuffer;
                        this->bindVertexBuffer(vb);
                    }
                    ::glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, attr.stride, (const GLvoid*) (GLintptr) attr.offset);
                    ORYOL_GL_CHECK_ERROR();
                    glExt::VertexAttribDivisor(attr.index, attr.divisor);
                    ORYOL_GL_CHECK_ERROR();
                    ::glEnableVertexAttribArray(attr.index);
                    ORYOL_GL_CHECK_ERROR();
                }
                else {
                    ::glDisableVertexAttribArray(attr.index);
                    ORYOL_GL_CHECK_ERROR();
                }
            }
        }
    #endif
}

//------------------------------------------------------------------------------
void
glRenderer::applyDrawState(drawState* ds) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != ds);
    o_assert_dbg(this->mshPool);
    
    this->curDrawState = ds;
    this->curProgramBundle = ds->prog;
    this->curMesh = this->mshPool->Lookup(ds->msh); // NOTE: curMesh can be nullptr now if mesh still loading!
    
    const DrawStateSetup& setup = ds->Setup;
    if (setup.DepthStencilState != this->depthStencilState) {
        this->applyDepthStencilState(setup.DepthStencilState);
    }
    if (setup.BlendState != this->blendState) {
        this->applyBlendState(setup.BlendState);
    }
    if (setup.RasterizerState != this->rasterizerState) {
        this->applyRasterizerState(setup.RasterizerState);
    }
    this->applyProgramBundle(this->curProgramBundle, setup.ProgramSelectionMask);
    if (this->curMesh) {
        this->applyMesh(this->curMesh, this->curProgramBundle);
    }
}

//------------------------------------------------------------------------------
void
glRenderer::applyTexture(int32 index, const texture* tex) {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->curProgramBundle);
    o_assert_dbg(tex);
    
    int32 samplerIndex = this->curProgramBundle->getSamplerIndex(index);
    GLuint glTexture = tex->glTex;
    GLenum glTarget = tex->glTarget;
    this->bindTexture(samplerIndex, glTarget, glTexture);
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariable(int32 index, const float32& val) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform1f(glLoc, val);
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariable(int32 index, const glm::vec2& val) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform2f(glLoc, val.x, val.y);
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariable(int32 index, const glm::vec3& val) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform3f(glLoc, val.x, val.y, val.z);
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariable(int32 index, const glm::vec4& val) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform4f(glLoc, val.x, val.y, val.z, val.w);
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariable(int32 index, const int32& val) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform1i(glLoc, val);
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariable(int32 index, const glm::ivec2& val) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform2i(glLoc, val.x, val.y);
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariable(int32 index, const glm::ivec3& val) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform3i(glLoc, val.x, val.y, val.z);
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariable(int32 index, const glm::ivec4& val) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform4i(glLoc, val.x, val.y, val.z, val.w);
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariable(int32 index, const glm::mat4& val) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniformMatrix4fv(glLoc, 1, GL_FALSE, glm::value_ptr(val));
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariable(int32 index, const glm::mat3& val) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniformMatrix3fv(glLoc, 1, GL_FALSE, glm::value_ptr(val));
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariable(int32 index, const glm::mat2& val) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniformMatrix2fv(glLoc, 1, GL_FALSE, glm::value_ptr(val));
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariableArray(int32 index, const float32* values, int32 numValues) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform1fv(glLoc, numValues, values);
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariableArray(int32 index, const glm::vec2* values, int32 numValues) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform2fv(glLoc, numValues, glm::value_ptr(*values));
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariableArray(int32 index, const glm::vec3* values, int32 numValues) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform3fv(glLoc, numValues, glm::value_ptr(*values));
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariableArray(int32 index, const glm::vec4* values, int32 numValues) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform4fv(glLoc, numValues, glm::value_ptr(*values));
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariableArray(int32 index, const int32* values, int32 numValues) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform1iv(glLoc, numValues, values);
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariableArray(int32 index, const glm::ivec2* values, int32 numValues) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform2iv(glLoc, numValues, glm::value_ptr(*values));
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariableArray(int32 index, const glm::ivec3* values, int32 numValues) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform3iv(glLoc, numValues, glm::value_ptr(*values));
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariableArray(int32 index, const glm::ivec4* values, int32 numValues) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniform4iv(glLoc, numValues, glm::value_ptr(*values));
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariableArray(int32 index, const glm::mat4* values, int32 numValues) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniformMatrix4fv(glLoc, numValues, GL_FALSE, glm::value_ptr(*values));
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariableArray(int32 index, const glm::mat3* values, int32 numValues) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniformMatrix3fv(glLoc, numValues, GL_FALSE, glm::value_ptr(*values));
}
    
//------------------------------------------------------------------------------
template<> void
glRenderer::applyVariableArray(int32 index, const glm::mat2* values, int32 numValues) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != this->curProgramBundle);
    o_assert_dbg(values && (numValues > 0));
    GLint glLoc = this->curProgramBundle->getUniformLocation(index);
    ::glUniformMatrix2fv(glLoc, numValues, GL_FALSE, glm::value_ptr(*values));
}

//------------------------------------------------------------------------------
void
glRenderer::clear(PixelChannel::Mask channels, const glm::vec4& color, float32 depth, uint8 stencil) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rtValid, "No render target set!");
    
    GLbitfield clearMask = 0;
    
    // update GL state
    if ((channels & PixelChannel::RGBA) != 0) {
        clearMask |= GL_COLOR_BUFFER_BIT;
        ::glClearColor(color.x, color.y, color.z, color.w);
        if ((channels & PixelChannel::RGBA) != this->blendState.ColorWriteMask) {
            this->blendState.ColorWriteMask = channels & PixelChannel::RGBA;
            ::glColorMask((channels & PixelChannel::R) != 0,
                          (channels & PixelChannel::G) != 0,
                          (channels & PixelChannel::B) != 0,
                          (channels & PixelChannel::A) != 0);
        }
    }
    if ((channels & PixelChannel::Depth) != 0) {
        clearMask |= GL_DEPTH_BUFFER_BIT;
        #if (ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
        ::glClearDepthf(depth);
        #else
        ::glClearDepth(depth);
        #endif
        if (!this->depthStencilState.DepthWriteEnabled) {
            this->depthStencilState.DepthWriteEnabled = true;
            ::glDepthMask(GL_TRUE);
        }
    }
    if ((channels & PixelChannel::Stencil) != 0) {
        clearMask |= GL_STENCIL_BUFFER_BIT;
        ::glClearStencil(stencil);
        if ((this->depthStencilState.StencilFront.StencilWriteMask != 0xFF) ||
            (this->depthStencilState.StencilBack.StencilWriteMask != 0xFF)) {
            
            this->depthStencilState.StencilFront.StencilWriteMask = 0xFF;
            this->depthStencilState.StencilBack.StencilWriteMask = 0xFF;
            ::glStencilMask(0xFF);
        }
    }
    
    // finally do the actual clear
    ::glClear(clearMask);
    ORYOL_GL_CHECK_ERROR();
}
    
//------------------------------------------------------------------------------
void
glRenderer::draw(const PrimitiveGroup& primGroup) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rtValid, "No render target set!");

    if (this->curMesh) {
        ORYOL_GL_CHECK_ERROR();
        const IndexType::Code indexType = this->curMesh->indexBufferAttrs.Type;
        if (indexType != IndexType::None) {
            // indexed geometry
            const int32 indexByteSize = IndexType::ByteSize(indexType);
            const GLvoid* indices = (const GLvoid*) (GLintptr) (primGroup.BaseElement * indexByteSize);
            ::glDrawElements(primGroup.PrimType, primGroup.NumElements, indexType, indices);
        }
        else {
            // non-indexed geometry
            ::glDrawArrays(primGroup.PrimType, primGroup.BaseElement, primGroup.NumElements);
        }
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glRenderer::draw(int32 primGroupIndex) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rtValid, "No render target set!");

    if (this->curMesh) {
        if (primGroupIndex >= this->curMesh->numPrimGroups) {
            // this may happen if trying to render a placeholder which doesn't
            // have as many materials as the original mesh, anyway, this isn't
            // a serious error
            return;
        }
        const PrimitiveGroup& primGroup = this->curMesh->primGroups[primGroupIndex];
        this->draw(primGroup);
    }
}

//------------------------------------------------------------------------------
void
glRenderer::drawInstanced(const PrimitiveGroup& primGroup, int32 numInstances) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rtValid, "No render target set!");

    if (this->curMesh) {
        ORYOL_GL_CHECK_ERROR();
        const IndexType::Code indexType = this->curMesh->indexBufferAttrs.Type;
        if (indexType != IndexType::None) {
            // indexed geometry
            const int32 indexByteSize = IndexType::ByteSize(indexType);
            const GLvoid* indices = (const GLvoid*) (GLintptr) (primGroup.BaseElement * indexByteSize);
            glExt::DrawElementsInstanced(primGroup.PrimType, primGroup.NumElements, indexType, indices, numInstances);
        }
        else {
            // non-indexed geometry
            glExt::DrawArraysInstanced(primGroup.PrimType, primGroup.BaseElement, primGroup.NumElements, numInstances);
        }
        ORYOL_GL_CHECK_ERROR();
    }
}
    
//------------------------------------------------------------------------------
void
glRenderer::drawInstanced(int32 primGroupIndex, int32 numInstances) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rtValid, "No render target set!");

    if (this->curMesh) {
        if (primGroupIndex >= this->curMesh->numPrimGroups) {
            // this may happen if trying to render a placeholder which doesn't
            // have as many materials as the original mesh, anyway, this isn't
            // a serious error
            return;
        }
        const PrimitiveGroup& primGroup = this->curMesh->primGroups[primGroupIndex];
        this->drawInstanced(primGroup, numInstances);
    }
}

//------------------------------------------------------------------------------
void
glRenderer::updateVertices(mesh* msh, const void* data, int32 numBytes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != msh);
    o_assert(numBytes > 0);
    
    const VertexBufferAttrs& attrs = msh->vertexBufferAttrs;
    const Usage::Code vbUsage = attrs.BufferUsage;
    o_assert_dbg((numBytes > 0) && (numBytes <= attrs.ByteSize()));
    o_assert_dbg((vbUsage == Usage::Stream) || (vbUsage == Usage::Dynamic) || (vbUsage == Usage::Static));
    
    uint8 slotIndex = msh->activeVertexBufferSlot;
    if (Usage::Stream == vbUsage) {
        // if usage is streaming, rotate slot index to next dynamic vertex buffer
        // to implement double/multi-buffering
        slotIndex++;
        if (slotIndex >= msh->numVertexBufferSlots) {
            slotIndex = 0;
        }
        msh->activeVertexBufferSlot = slotIndex;
        msh->setActiveVAOSlot(slotIndex);
    }
    
    GLuint vb = msh->glVertexBuffers[slotIndex];
    this->bindVertexBuffer(vb);
    ::glBufferSubData(GL_ARRAY_BUFFER, 0, numBytes, data);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::readPixels(displayMgr* displayManager, void* buf, int32 bufNumBytes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(displayManager);
    o_assert_dbg((nullptr != buf) && (bufNumBytes > 0));
    
    GLsizei width, height;
    GLenum format, type;
    if (nullptr == this->curRenderTarget) {
        const DisplayAttrs& attrs = displayManager->GetDisplayAttrs();
        width  = attrs.FramebufferWidth;
        height = attrs.FramebufferHeight;
        format = glTypes::AsGLTexImageFormat(attrs.ColorPixelFormat);
        type   = glTypes::AsGLTexImageType(attrs.ColorPixelFormat);
        o_assert((width & 3) == 0);
        o_assert(bufNumBytes >= (width * height * PixelFormat::ByteSize(attrs.ColorPixelFormat)));
    }
    else {
        const TextureAttrs& attrs = this->curRenderTarget->textureAttrs;
        width  = attrs.Width;
        height = attrs.Height;
        format = glTypes::AsGLTexImageFormat(attrs.ColorFormat);
        type   = glTypes::AsGLTexImageType(attrs.ColorFormat);
        o_assert((width & 3) == 0);
        o_assert(bufNumBytes >= (width * height * PixelFormat::ByteSize(attrs.ColorFormat)));
    }
    ::glReadPixels(0, 0, width, height, format, type, buf);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::invalidateMeshState() {
    o_assert_dbg(this->valid);

    if (glExt::HasExtension(glExt::VertexArrayObject)) {
        // NOTE: it is essential that the current vertex array object
        // is unbound before modifying the GL_ELEMENT_ARRAY_BUFFER as this
        // would bind the next index buffer to the previous VAO!
        glExt::BindVertexArray(0);
    }
    ::glBindBuffer(GL_ARRAY_BUFFER, 0);
    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    this->vertexArrayObject = 0;
    this->vertexBuffer = 0;
    this->indexBuffer = 0;
}

//------------------------------------------------------------------------------
void
glRenderer::bindVertexBuffer(GLuint vb) {
    o_assert_dbg(this->valid);

    if (vb != this->vertexBuffer) {
        this->vertexArrayObject = 0;
        this->vertexBuffer = vb;
        ::glBindBuffer(GL_ARRAY_BUFFER, vb);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glRenderer::bindIndexBuffer(GLuint ib) {
    o_assert_dbg(this->valid);

    if (ib != this->indexBuffer) {
        this->vertexArrayObject = 0;
        this->indexBuffer = ib;
        ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
        ORYOL_GL_CHECK_ERROR();
    }
}
    
//------------------------------------------------------------------------------
void
glRenderer::bindVertexArrayObject(GLuint vao) {
    o_assert_dbg(this->valid);

    if (vao != this->vertexArrayObject) {
        this->vertexBuffer = 0;
        this->indexBuffer = 0;
        this->vertexArrayObject = vao;
        glExt::BindVertexArray(vao);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glRenderer::invalidateProgramState() {
    o_assert_dbg(this->valid);

    ::glUseProgram(0);
    ORYOL_GL_CHECK_ERROR();
    this->program = 0;
}
    
//------------------------------------------------------------------------------
void
glRenderer::useProgram(GLuint prog) {
    o_assert_dbg(this->valid);

    if (prog != this->program) {
        this->program = prog;
        ::glUseProgram(prog);
        ORYOL_GL_CHECK_ERROR();
    }
}
    
//------------------------------------------------------------------------------
void
glRenderer::invalidateTextureState() {
    o_assert_dbg(this->valid);

    for (int32 i = 0; i < MaxTextureSamplers; i++) {
        this->samplers2D[i] = 0;
        this->samplersCube[i] = 0;
    }
}
    
//------------------------------------------------------------------------------
void
glRenderer::bindTexture(int32 samplerIndex, GLenum target, GLuint tex) {
    o_assert_dbg(this->valid);
    o_assert_range_dbg(samplerIndex, MaxTextureSamplers);
    o_assert_dbg((target == GL_TEXTURE_2D) || (target == GL_TEXTURE_CUBE_MAP));
    
    GLuint* samplers = (GL_TEXTURE_2D == target) ? this->samplers2D : this->samplersCube;
    if (tex != samplers[samplerIndex]) {
        samplers[samplerIndex] = tex;
        ::glActiveTexture(GL_TEXTURE0 + samplerIndex);
        ORYOL_GL_CHECK_ERROR();
        ::glBindTexture(target, tex);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glRenderer::setupDepthStencilState() {
    o_assert_dbg(this->valid);
    
    this->depthStencilState = DepthStencilState();
    
    ::glEnable(GL_DEPTH_TEST);
    ::glDepthFunc(GL_ALWAYS);
    ::glDepthMask(GL_FALSE);
    ::glDisable(GL_STENCIL_TEST);
    ::glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
    ::glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    ::glStencilMask(0xFFFFFFFF);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::applyStencilState(const StencilState& newState, const StencilState& curState, GLenum glFace) {
    o_assert_dbg(this->valid);
    
    const CompareFunc::Code cmpFunc = newState.StencilCmpFunc;
    const uint32 readMask = newState.StencilReadMask;
    const int32 stencilRef = newState.StencilRef;
    if ((cmpFunc != curState.StencilCmpFunc) || (readMask != curState.StencilReadMask) || (stencilRef != curState.StencilRef)) {
        o_assert_range_dbg(cmpFunc, CompareFunc::NumCompareFuncs);
        ::glStencilFuncSeparate(glFace, mapCompareFunc[cmpFunc], stencilRef, readMask);
    }
    
    const StencilOp::Code sFailOp = newState.StencilFailOp;
    const StencilOp::Code dFailOp = newState.DepthFailOp;
    const StencilOp::Code passOp = newState.DepthStencilPassOp;
    if ((sFailOp != curState.StencilFailOp) || (dFailOp != curState.DepthFailOp) || (passOp  != curState.DepthStencilPassOp)) {
        o_assert_range_dbg(sFailOp, StencilOp::NumStencilOperations);
        o_assert_range_dbg(dFailOp, StencilOp::NumStencilOperations);
        o_assert_range_dbg(passOp, StencilOp::NumStencilOperations);
        ::glStencilOpSeparate(glFace, mapStencilOp[sFailOp], mapStencilOp[dFailOp], mapStencilOp[passOp]);
    }
    
    const uint32 writeMask = newState.StencilWriteMask;
    if (writeMask != curState.StencilWriteMask) {
        ::glStencilMaskSeparate(glFace, writeMask);
    }
}
    
//------------------------------------------------------------------------------
void
glRenderer::applyDepthStencilState(const DepthStencilState& newState) {
    o_assert_dbg(this->valid);
    
    const DepthStencilState& curState = this->depthStencilState;
    
    // apply depth state if changed
    bool depthChanged = false;
    if (curState.DepthStateHash != newState.DepthStateHash) {
        const CompareFunc::Code depthCmpFunc = newState.DepthCmpFunc;
        const bool depthWriteEnabled = newState.DepthWriteEnabled;
        if (depthCmpFunc != curState.DepthCmpFunc) {
            o_assert_range_dbg(depthCmpFunc, CompareFunc::NumCompareFuncs);
            ::glDepthFunc(mapCompareFunc[depthCmpFunc]);
        }
        if (depthWriteEnabled != curState.DepthWriteEnabled) {
            ::glDepthMask(depthWriteEnabled);
        }
        depthChanged = true;
    }
    
    // apply front and back stencil state
    bool frontChanged = false;
    const StencilState& newFront = newState.StencilFront;
    const StencilState& curFront = curState.StencilFront;
    if (curFront.Hash != newFront.Hash) {
        frontChanged = true;
        this->applyStencilState(newFront, curFront, GL_FRONT);
    }
    bool backChanged = false;
    const StencilState& newBack = newState.StencilBack;
    const StencilState& curBack = curState.StencilBack;
    if (curBack.Hash != newBack.Hash) {
        backChanged = true;
        this->applyStencilState(newBack, curBack, GL_BACK);
    }
    
    // enable/disable stencil state?
    if (frontChanged || backChanged) {
        if (newFront.StencilEnabled || newBack.StencilEnabled) {
            ::glEnable(GL_STENCIL_TEST);
        }
        else {
            ::glDisable(GL_STENCIL_TEST);
        }
    }
    
    // update state cache
    if (depthChanged || frontChanged || backChanged) {
        this->depthStencilState = newState;
    }
}

//------------------------------------------------------------------------------
void
glRenderer::setupBlendState() {
    o_assert_dbg(this->valid);

    this->blendState = BlendState();
    ::glDisable(GL_BLEND);
    ::glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
    ::glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    ::glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    ORYOL_GL_CHECK_ERROR();
}
    
//------------------------------------------------------------------------------
void
glRenderer::applyBlendState(const BlendState& bs) {
    o_assert_dbg(this->valid);
    
    if (bs.BlendEnabled != this->blendState.BlendEnabled) {
        if (bs.BlendEnabled) {
            ::glEnable(GL_BLEND);
        }
        else {
            ::glDisable(GL_BLEND);
        }
    }
    
    if ((bs.SrcFactorRGB != this->blendState.SrcFactorRGB) ||
        (bs.DstFactorRGB != this->blendState.DstFactorRGB) ||
        (bs.SrcFactorAlpha != this->blendState.SrcFactorAlpha) ||
        (bs.DstFactorAlpha != this->blendState.DstFactorAlpha)) {
        
        o_assert_range_dbg(bs.SrcFactorRGB, BlendFactor::NumBlendFactors);
        o_assert_range_dbg(bs.DstFactorRGB, BlendFactor::NumBlendFactors);
        o_assert_range_dbg(bs.SrcFactorAlpha, BlendFactor::NumBlendFactors);
        o_assert_range_dbg(bs.DstFactorAlpha, BlendFactor::NumBlendFactors);
        
        ::glBlendFuncSeparate(mapBlendFactor[bs.SrcFactorRGB],
                              mapBlendFactor[bs.DstFactorRGB],
                              mapBlendFactor[bs.SrcFactorAlpha],
                              mapBlendFactor[bs.DstFactorAlpha]);
    }
    if ((bs.OpRGB != this->blendState.OpRGB) ||
        (bs.OpAlpha != this->blendState.OpAlpha)) {
        
        o_assert_range_dbg(bs.OpRGB, BlendOperation::NumBlendOperations);
        o_assert_range_dbg(bs.OpAlpha, BlendOperation::NumBlendOperations);
        
        ::glBlendEquationSeparate(mapBlendOp[bs.OpRGB], mapBlendOp[bs.OpAlpha]);
    }
    
    if (bs.ColorWriteMask != this->blendState.ColorWriteMask) {
        ::glColorMask((bs.ColorWriteMask & PixelChannel::R) != 0,
                      (bs.ColorWriteMask & PixelChannel::G) != 0,
                      (bs.ColorWriteMask & PixelChannel::B) != 0,
                      (bs.ColorWriteMask & PixelChannel::A) != 0);
    }
    
    this->blendState = bs;
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderer::setupRasterizerState() {
    o_assert_dbg(this->valid);
    
    this->rasterizerState = RasterizerState();
    
    ::glDisable(GL_CULL_FACE);
    ::glFrontFace(GL_CW);
    ::glCullFace(GL_BACK);
    ::glDisable(GL_POLYGON_OFFSET_FILL);
    ::glDisable(GL_SCISSOR_TEST);
    ::glEnable(GL_DITHER);
    #if !(ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
    ::glEnable(GL_MULTISAMPLE);
    #endif
    ORYOL_GL_CHECK_ERROR();
}
    
//------------------------------------------------------------------------------
void
glRenderer::applyRasterizerState(const RasterizerState& newState) {
    o_assert_dbg(this->valid);

    const RasterizerState& curState = this->rasterizerState;
    
    const bool cullFaceEnabled = newState.CullFaceEnabled;
    if (cullFaceEnabled != curState.CullFaceEnabled) {
        if (cullFaceEnabled) {
            ::glEnable(GL_CULL_FACE);
        }
        else {
            ::glDisable(GL_CULL_FACE);
        }
    }
    const Face::Code cullFace = newState.CullFace;
    if (cullFace != curState.CullFace) {
        o_assert_range_dbg(cullFace, Face::NumFaceCodes);
        ::glCullFace(mapCullFace[cullFace]);
    }
    const bool depthOffsetEnabled = newState.DepthOffsetEnabled;
    if (depthOffsetEnabled != curState.DepthOffsetEnabled) {
        if (depthOffsetEnabled) {
            ::glEnable(GL_POLYGON_OFFSET_FILL);
        }
        else {
            ::glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }
    const bool scissorTestEnabled = newState.ScissorTestEnabled;
    if (scissorTestEnabled != curState.ScissorTestEnabled) {
        if (scissorTestEnabled) {
            ::glEnable(GL_SCISSOR_TEST);
        }
        else {
            ::glDisable(GL_SCISSOR_TEST);
        }
    }
    const bool ditherEnabled = newState.DitherEnabled;
    if (ditherEnabled != curState.DitherEnabled) {
        if (ditherEnabled) {
            ::glEnable(GL_DITHER);
        }
        else {
            ::glDisable(GL_DITHER);
        }
    }
    #if !(ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
    const bool msaaEnabled = newState.MultisampleEnabled;
    if (msaaEnabled != curState.MultisampleEnabled) {
        if (msaaEnabled) {
            ::glEnable(GL_MULTISAMPLE);
        }
        else {
            ::glDisable(GL_MULTISAMPLE);
        }
    }
    #endif
    this->rasterizerState = newState;
    ORYOL_GL_CHECK_ERROR();
}

} // namespace _priv
} // namespace Oryol