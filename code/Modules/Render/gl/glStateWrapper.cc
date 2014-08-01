//------------------------------------------------------------------------------
//  glStateWrapper.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Render/gl/gl_impl.h"
#include "glStateWrapper.h"
#include "Render/gl/glExt.h"
#include "Core/Memory/Memory.h"
#include "Render/Core/mesh.h"
#include "Render/Core/programBundle.h"
#include "Render/Core/drawState.h"
#include "glm/vec4.hpp"

namespace Oryol {
namespace Render {

using namespace Core;

GLenum glStateWrapper::mapCompareFunc[CompareFunc::NumCompareFuncs] = {
    GL_NEVER,
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS
};

GLenum glStateWrapper::mapStencilOp[StencilOp::NumStencilOperations] = {
    GL_KEEP,
    GL_ZERO,
    GL_REPLACE,
    GL_INCR,
    GL_DECR,
    GL_INVERT,
    GL_INCR_WRAP,
    GL_DECR_WRAP
};

GLenum glStateWrapper::mapBlendFactor[BlendFactor::NumBlendFactors] = {
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

GLenum glStateWrapper::mapBlendOp[BlendOperation::NumBlendOperations] = {
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
};

GLenum glStateWrapper::mapCullFace[Face::NumFaceCodes] = {
    GL_FRONT,
    GL_BACK,
    GL_FRONT_AND_BACK,
};

//------------------------------------------------------------------------------
glStateWrapper::glStateWrapper() :
isValid(false),
#if ORYOL_MACOS
globalVAO(0),
#endif
curScissorX(0),
curScissorY(0),
curScissorWidth(-1),
curScissorHeight(-1),
curBlendColorR(0.0f),
curBlendColorG(0.0f),
curBlendColorB(0.0f),
curBlendColorA(0.0f),
curViewPortX(0),
curViewPortY(0),
curViewPortWidth(-1),
curViewPortHeight(-1),
curVertexBuffer(0),
curIndexBuffer(0),
curVertexArrayObject(0),
curProgram(0)
{
    for (int32 i = 0; i < MaxTextureSamplers; i++) {
        this->samplers2D[i] = 0;
        this->samplersCube[i] = 0;
    }
}

//------------------------------------------------------------------------------
glStateWrapper::~glStateWrapper() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void
glStateWrapper::Setup() {
    o_assert(!this->isValid);
    this->isValid = true;
    
    #if ORYOL_USE_GLGETATTRIBLOCATION
    Log::Warn("glStateWrapper: ORYOL_USE_GLGETATTRIBLOCATION is ON\n");
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
glStateWrapper::Discard() {
    o_assert(this->isValid);
    #if ORYOL_MACOS
    ::glDeleteVertexArrays(1, &this->globalVAO);
    this->globalVAO = 0;
    #endif
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
glStateWrapper::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
glStateWrapper::ApplyScissorRect(int32 x, int32 y, int32 width, int32 height) {
    if ((x != this->curScissorX) ||
        (y != this->curScissorY) ||
        (width != this->curScissorWidth) ||
        (height != this->curScissorHeight)) {
        
        this->curScissorX = x;
        this->curScissorY = y;
        this->curScissorWidth = width;
        this->curScissorHeight = height;
        ::glScissor(x, y, width, height);
    }
}
    
//------------------------------------------------------------------------------
void
glStateWrapper::ApplyBlendColor(float32 r, float32 g, float32 b, float32 a) {
    if ((r != this->curBlendColorR) ||
        (g != this->curBlendColorG) ||
        (b != this->curBlendColorB) ||
        (a != this->curBlendColorA)) {
        
        this->curBlendColorR = r;
        this->curBlendColorG = g;
        this->curBlendColorB = b;
        this->curBlendColorA = a;
        ::glBlendColor(r, g, b, a);
    }
}
    
//------------------------------------------------------------------------------
void
glStateWrapper::ApplyViewPort(int32 x, int32 y, int32 width, int32 height) {
    if ((x != this->curViewPortX) ||
        (y != this->curViewPortY) ||
        (width != this->curViewPortWidth) ||
        (height != this->curViewPortHeight)) {
        
        this->curViewPortX = x;
        this->curViewPortY = y;
        this->curViewPortWidth = width;
        this->curViewPortHeight = height;
        ::glViewport(x, y, width, height);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::ApplyRenderTargetState(int32 rtWidth, int32 rtHeight) {
    
    // set viewport to cover whole screen
    this->ApplyViewPort(0, 0, rtWidth, rtHeight);
    
    // disable scissor test
    if (this->curRasterizerState.ScissorTestEnabled) {
        this->curRasterizerState.ScissorTestEnabled = false;
        ::glDisable(GL_SCISSOR_TEST);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::ApplyDrawState(const drawState* ds) {
    o_assert_dbg(nullptr != ds);
    const DrawStateSetup& setup = ds->GetSetup();
    if (setup.DepthStencilState != this->curDepthStencilState) {
        this->applyDepthStencilState(setup.DepthStencilState);
    }
    if (setup.BlendState != this->curBlendState) {
        this->applyBlendState(setup.BlendState);
    }
    if (setup.RasterizerState != this->curRasterizerState) {
        this->applyRasterizerState(setup.RasterizerState);
    }
    programBundle* pb = ds->getProgramBundle();
    this->applyProgram(pb, setup.ProgramSelectionMask);
    this->applyMesh(ds->getMesh(), pb);
}

//------------------------------------------------------------------------------
void
glStateWrapper::applyProgram(programBundle* progBundle, uint32 mask) {
    progBundle->selectProgram(mask);
    GLuint glProg = progBundle->getProgram();
    o_assert_dbg(0 != glProg);
    this->UseProgram(glProg);
}
    
//------------------------------------------------------------------------------
void
glStateWrapper::applyMesh(const mesh* msh, const programBundle* progBundle) {
    
    // bind the mesh
    if (nullptr == msh) {
        this->InvalidateMeshState();
    }
    else {
        // FIXME: record and compare against a 'current mesh pointer' whether
        // mesh state must be reapplied
        const uint8 vaoIndex = msh->getActiveVAOSlot();
    
#if ORYOL_USE_GLGETATTRIBLOCATION
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
            if (attr.vertexBuffer != vb) {
                vb = attr.vertexAttr;
                this->BindVertexBuffer(vb);
                ORYOL_GL_CHECK_ERROR();
            }
            const GLint glAttribIndex = progBundle->getAttribLocation((VertexAttr::Code)i);
            if (-1 != glAttribIndex) {
                ::glVertexAttribPointer(glAttribIndex, attr.size, attr.type, attr.normalized, attr.stride, (const GLvoid*) (GLintptr) attr.offset);
                ORYOL_GL_CHECK_ERROR();
                glExt::VertexAttribDivisor(attr.index, attr.divisor);
                ORYOL_GL_CHECK_ERROR();
                ::glEnableVertexAttribArray(glAttribIndex);
                ORYOL_GL_CHECK_ERROR();
                maxUsedAttrib++;
            }
        }
        int32 maxAttribs = glInfo::Int(MaxVertexAttribs);
        if (VertexAttr::NumVertexAttrs < maxAttribs) {
            maxAttribs = VertexAttr::NumVertexAttrs;
        }
        for (int32 i = maxUsedAttrib; i < maxAttribs; i++) {
            ::glDisableVertexAttribArray(i);
        }
#else
        if (glExt::HasExtension(glExt::VertexArrayObject)) {
            GLuint vao = msh->glGetVAO(vaoIndex);
            o_assert_dbg(0 != vao);
            this->BindVertexArrayObject(vao);
        }
        else {
            GLuint vb = 0;
            const GLuint ib = msh->glGetIndexBuffer();
            this->BindIndexBuffer(ib);
            ORYOL_GL_CHECK_ERROR();
            for (uint8 attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
                const glVertexAttr& attr = msh->glAttr(vaoIndex, attrIndex);
                if (attr.enabled) {
                    if (attr.vertexBuffer != vb) {
                        vb = attr.vertexBuffer;
                        this->BindVertexBuffer(vb);
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
}
    
//------------------------------------------------------------------------------
void
glStateWrapper::setupDepthStencilState() {
    
    this->curDepthStencilState = DepthStencilState();

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
glStateWrapper::applyStencilState(const StencilState& newState, const StencilState& curState, GLenum glFace) {

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
glStateWrapper::applyDepthStencilState(const DepthStencilState& newState) {

    const DepthStencilState& curState = this->curDepthStencilState;

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
        this->curDepthStencilState = newState;
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::setupBlendState() {
    this->curBlendState = BlendState();
    ::glDisable(GL_BLEND);
    ::glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
    ::glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    ::glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glStateWrapper::applyBlendState(const BlendState& bs) {

    if (bs.BlendEnabled != this->curBlendState.BlendEnabled) {
        if (bs.BlendEnabled) {
            ::glEnable(GL_BLEND);
        }
        else {
            ::glDisable(GL_BLEND);
        }
    }
    
    if ((bs.SrcFactorRGB != this->curBlendState.SrcFactorRGB) ||
        (bs.DstFactorRGB != this->curBlendState.DstFactorRGB) ||
        (bs.SrcFactorAlpha != this->curBlendState.SrcFactorAlpha) ||
        (bs.DstFactorAlpha != this->curBlendState.DstFactorAlpha)) {
        
        o_assert_range_dbg(bs.SrcFactorRGB, BlendFactor::NumBlendFactors);
        o_assert_range_dbg(bs.DstFactorRGB, BlendFactor::NumBlendFactors);
        o_assert_range_dbg(bs.SrcFactorAlpha, BlendFactor::NumBlendFactors);
        o_assert_range_dbg(bs.DstFactorAlpha, BlendFactor::NumBlendFactors);

        ::glBlendFuncSeparate(mapBlendFactor[bs.SrcFactorRGB],
                              mapBlendFactor[bs.DstFactorRGB],
                              mapBlendFactor[bs.SrcFactorAlpha],
                              mapBlendFactor[bs.DstFactorAlpha]);
    }
    if ((bs.OpRGB != this->curBlendState.OpRGB) ||
        (bs.OpAlpha != this->curBlendState.OpAlpha)) {

        o_assert_range_dbg(bs.OpRGB, BlendOperation::NumBlendOperations);
        o_assert_range_dbg(bs.OpAlpha, BlendOperation::NumBlendOperations);

        ::glBlendEquationSeparate(mapBlendOp[bs.OpRGB], mapBlendOp[bs.OpAlpha]);
    }

    if (bs.ColorWriteMask != this->curBlendState.ColorWriteMask) {
        ::glColorMask((bs.ColorWriteMask & Channel::R) != 0,
                      (bs.ColorWriteMask & Channel::G) != 0,
                      (bs.ColorWriteMask & Channel::B) != 0,
                      (bs.ColorWriteMask & Channel::A) != 0);
    }
    
    this->curBlendState = bs;
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glStateWrapper::setupRasterizerState() {
    
    this->curRasterizerState = RasterizerState();
    
    ::glDisable(GL_CULL_FACE);
    ::glFrontFace(GL_CW);
    ::glCullFace(GL_BACK);
    ::glDisable(GL_POLYGON_OFFSET_FILL);
    ::glDisable(GL_SCISSOR_TEST);
    ::glDisable(GL_DITHER);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glStateWrapper::applyRasterizerState(const RasterizerState& newState) {
    const RasterizerState& curState = this->curRasterizerState;

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
    this->curRasterizerState = newState;
    ORYOL_GL_CHECK_ERROR();        
}

//------------------------------------------------------------------------------
void
glStateWrapper::InvalidateMeshState() {
    if (glExt::HasExtension(glExt::VertexArrayObject)) {
        // NOTE: it is essential that the current vertex array object
        // is unbound before modifying the GL_ELEMENT_ARRAY_BUFFER as this
        // would bind the next index buffer to the previous VAO!
        glExt::BindVertexArray(0);
    }
    ::glBindBuffer(GL_ARRAY_BUFFER, 0);
    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    this->curVertexArrayObject = 0;
    this->curVertexBuffer = 0;
    this->curIndexBuffer = 0;
}

//------------------------------------------------------------------------------
void
glStateWrapper::BindVertexBuffer(GLuint vb) {
    if (vb != this->curVertexBuffer) {
        this->curVertexArrayObject = 0;
        this->curVertexBuffer = vb;
        ::glBindBuffer(GL_ARRAY_BUFFER, vb);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::BindIndexBuffer(GLuint ib) {
    if (ib != this->curIndexBuffer) {
        this->curVertexArrayObject = 0;
        this->curIndexBuffer = ib;
        ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::BindVertexArrayObject(GLuint vao) {
    if (vao != this->curVertexArrayObject) {
        this->curVertexBuffer = 0;
        this->curIndexBuffer = 0;
        this->curVertexArrayObject = vao;
        glExt::BindVertexArray(vao);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::InvalidateProgramState() {
    ::glUseProgram(0);
    ORYOL_GL_CHECK_ERROR();
    this->curProgram = 0;
}

//------------------------------------------------------------------------------
void
glStateWrapper::UseProgram(GLuint prog) {
    if (prog != this->curProgram) {
        this->curProgram = prog;
        ::glUseProgram(prog);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::InvalidateTextureState() {
    for (int32 i = 0; i < MaxTextureSamplers; i++) {
        this->samplers2D[i] = 0;
        this->samplersCube[i] = 0;
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::BindTexture(int32 samplerIndex, GLenum target, GLuint tex) {
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
glStateWrapper::Clear(Channel::Mask channels, const glm::vec4& color, float32 depth, uint8 stencil) {

    GLbitfield clearMask = 0;

    // update GL state
    if ((channels & Channel::RGBA) != 0) {
        
        clearMask |= GL_COLOR_BUFFER_BIT;
        ::glClearColor(color.x, color.y, color.z, color.w);
        
        if ((channels & Channel::RGBA) != this->curBlendState.ColorWriteMask) {
            this->curBlendState.ColorWriteMask = channels & Channel::RGBA;
            ::glColorMask((channels & Channel::R) != 0,
                          (channels & Channel::G) != 0,
                          (channels & Channel::B) != 0,
                          (channels & Channel::A) != 0);
        }
    }
    if ((channels & Channel::Depth) != 0) {
    
        clearMask |= GL_DEPTH_BUFFER_BIT;
        #if ORYOL_OPENGLES2
        ::glClearDepthf(depth);
        #else
        ::glClearDepth(depth);
        #endif

        if (!this->curDepthStencilState.DepthWriteEnabled) {
            this->curDepthStencilState.DepthWriteEnabled = true;
            ::glDepthMask(GL_TRUE);
        }
    }
    if ((channels & Channel::Stencil) != 0) {
    
        clearMask |= GL_STENCIL_BUFFER_BIT;
        ::glClearStencil(stencil);
        
        if ((this->curDepthStencilState.StencilFront.StencilWriteMask != 0xFF) ||
            (this->curDepthStencilState.StencilBack.StencilWriteMask != 0xFF)) {
            
            this->curDepthStencilState.StencilFront.StencilWriteMask = 0xFF;
            this->curDepthStencilState.StencilBack.StencilWriteMask = 0xFF;
            ::glStencilMask(0xFF);
        }
    }
    
    // finally do the actual clear
    ::glClear(clearMask);
    ORYOL_GL_CHECK_ERROR();
}

} // namespace Render
} // namespace Oryol
