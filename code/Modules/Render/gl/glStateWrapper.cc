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
curDepthOffsetFactor(0.0f),
curDepthOffsetUnits(0.0f),
curScissorLeft(0),
curScissorBottom(0),
curScissorWidth(-1),
curScissorHeight(-1),
curBlendColorR(0.0f),
curBlendColorG(0.0f),
curBlendColorB(0.0f),
curBlendColorA(0.0f),
curClearColorR(0.0f),
curClearColorG(0.0f),
curClearColorB(0.0f),
curClearColorA(0.0f),
curClearDepth(1.0f),
curClearStencil(0),
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
    this->setupJumpTable();
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
        int32 maxAttribs = glExt::GetMaxVertexAttribs();
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

    const CompareFunc::Code cmpFunc = newState.GetCompareFunc();
    const uint32 readMask = newState.GetReadMask();
    const int32 stencilRef = newState.GetRef();
    if ((cmpFunc != curState.GetCompareFunc()) || (readMask != curState.GetReadMask()) || (stencilRef != curState.GetRef())) {
        ::glStencilFuncSeparate(glFace, mapCompareFunc[cmpFunc], stencilRef, readMask);
    }
    
    const StencilOp::Code sFailOp = newState.GetStencilFailOp();
    const StencilOp::Code dFailOp = newState.GetDepthFailOp();
    const StencilOp::Code passOp = newState.GetDepthStencilPassOp();
    if ((sFailOp != curState.GetStencilFailOp()) || (dFailOp != curState.GetDepthFailOp()) || (passOp  != curState.GetDepthStencilPassOp())) {
        ::glStencilOpSeparate(glFace, mapStencilOp[sFailOp], mapStencilOp[dFailOp], mapStencilOp[passOp]);
    }
    
    const uint32 writeMask = newState.GetWriteMask();
    if (writeMask != curState.GetWriteMask()) {
        ::glStencilMask(writeMask);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::applyDepthStencilState(const DepthStencilState& newState) {

    const DepthStencilState& curState = this->curDepthStencilState;

    // apply depth state if changed
    bool depthChanged = false;
    if (curState.GetDepthStateHash() != newState.GetDepthStateHash()) {
        const CompareFunc::Code depthCmpFunc = newState.GetDepthCompareFunc();
        const bool depthWriteEnabled = newState.GetDepthWriteEnabled();
        if (depthCmpFunc != curState.GetDepthCompareFunc()) {
            ::glDepthFunc(mapCompareFunc[depthCmpFunc]);
        }
        if (depthWriteEnabled != curState.GetDepthWriteEnabled()) {
            ::glDepthMask(depthWriteEnabled);
        }
        depthChanged = true;
    }
    
    // apply front and back stencil state
    bool frontChanged = false;
    const StencilState& newFront = newState.StencilState(Face::Front);
    const StencilState& curFront = curState.StencilState(Face::Front);
    if (curFront.GetHash() != newFront.GetHash()) {
        frontChanged = true;
        this->applyStencilState(newFront, curFront, GL_FRONT);
    }
    bool backChanged = false;
    const StencilState& newBack = newState.StencilState(Face::Back);
    const StencilState& curBack = curState.StencilState(Face::Back);
    if (curBack.GetHash() != newBack.GetHash()) {
        backChanged = true;
        this->applyStencilState(newBack, curBack, GL_BACK);
    }
    
    // enable/disable stencil state?
    if (frontChanged || backChanged) {
        if (newFront.GetEnabled() || newBack.GetEnabled()) {
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

    const bool blendEnabled = bs.GetEnabled();
    if (blendEnabled != this->curBlendState.GetEnabled()) {
        if (blendEnabled) {
            ::glEnable(GL_BLEND);
        }
        else {
            ::glDisable(GL_BLEND);
        }
    }
    
    const BlendFactor::Code srcRgb = bs.GetSrcFactorRGB();
    const BlendFactor::Code dstRgb = bs.GetDstFactorRGB();
    const BlendFactor::Code srcAlpha = bs.GetSrcFactorAlpha();
    const BlendFactor::Code dstAlpha = bs.GetDstFactorAlpha();
    if ((srcRgb != this->curBlendState.GetSrcFactorRGB()) ||
        (dstRgb != this->curBlendState.GetDstFactorRGB()) ||
        (srcAlpha != this->curBlendState.GetSrcFactorAlpha()) ||
        (dstAlpha != this->curBlendState.GetDstFactorAlpha())) {
        
        ::glBlendFuncSeparate(mapBlendFactor[srcRgb],
                              mapBlendFactor[dstRgb],
                              mapBlendFactor[srcAlpha],
                              mapBlendFactor[dstAlpha]);
    }

    const BlendOperation::Code rgbOp = bs.GetOpRGB();
    const BlendOperation::Code alphaOp = bs.GetOpAlpha();
    if ((rgbOp != this->curBlendState.GetOpRGB()) ||
        (alphaOp != this->curBlendState.GetOpAlpha())) {

        ::glBlendEquationSeparate(mapBlendOp[rgbOp], mapBlendOp[alphaOp]);
    }

    const ColorWriteMask::Code colorMask = bs.GetColorWriteMask();
    if (colorMask != this->curBlendState.GetColorWriteMask()) {
        ::glColorMask((colorMask & ColorWriteMask::R) != 0,
                      (colorMask & ColorWriteMask::G) != 0,
                      (colorMask & ColorWriteMask::B) != 0,
                      (colorMask & ColorWriteMask::A) != 0);
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

    const bool cullFaceEnabled = newState.GetCullFaceEnabled();
    if (cullFaceEnabled != curState.GetCullFaceEnabled()) {
        if (cullFaceEnabled) {
            ::glEnable(GL_CULL_FACE);
        }
        else {
            ::glDisable(GL_CULL_FACE);
        }
    }
    const Face::Code cullFace = newState.GetCullFace();
    if (cullFace != curState.GetCullFace()) {
        ::glCullFace(mapCullFace[cullFace]);
    }
    const bool depthOffsetEnabled = newState.GetDepthOffsetEnabled();
    if (depthOffsetEnabled != curState.GetDepthOffsetEnabled()) {
        if (depthOffsetEnabled) {
            ::glEnable(GL_POLYGON_OFFSET_FILL);
        }
        else {
            ::glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }
    const bool scissorTestEnabled = newState.GetScissorTestEnabled();
    if (scissorTestEnabled != curState.GetScissorTestEnabled()) {
        if (scissorTestEnabled) {
            ::glEnable(GL_SCISSOR_TEST);
        }
        else {
            ::glDisable(GL_SCISSOR_TEST);
        }
    }
    const bool ditherEnabled = newState.GetDitherEnabled();
    if (ditherEnabled != curState.GetDitherEnabled()) {
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
glStateWrapper::onDepthOffset(const State::Vector& input) {
    const GLfloat factor = input.val[0].f;
    const GLfloat units  = input.val[1].f;
    if ((factor != this->curDepthOffsetFactor) || (units != this->curDepthOffsetUnits)) {
        this->curDepthOffsetFactor = factor;
        this->curDepthOffsetUnits = units;
        ::glPolygonOffset(factor, units);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onScissorRect(const State::Vector& input) {
    const GLint left = input.val[0].i;
    const GLint bottom = input.val[1].i;
    const GLsizei width = input.val[2].i;
    const GLsizei height = input.val[3].i;
    if ((left != this->curScissorLeft) || (bottom != this->curScissorBottom) ||
        (width != this->curScissorWidth) || (height != this->curScissorHeight)) {
        this->curScissorLeft = left;
        this->curScissorBottom = bottom;
        this->curScissorWidth = width;
        this->curScissorHeight = height;
        ::glScissor(left, bottom, width, height);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onBlendColor(const State::Vector& input) {
    const GLclampf r = input.val[0].f;
    const GLclampf g = input.val[1].f;
    const GLclampf b = input.val[2].f;
    const GLclampf a = input.val[3].f;
    if ((r != this->curBlendColorR) || (g != this->curBlendColorG) || (b != this->curBlendColorB) || (a != this->curBlendColorA)) {
        this->curBlendColorR = r;
        this->curBlendColorG = g;
        this->curBlendColorB = b;
        this->curBlendColorA = a;
        ::glBlendColor(r, g, b, a);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onClearColor(const State::Vector& input) {
    const GLclampf r = input.val[0].f;
    const GLclampf g = input.val[1].f;
    const GLclampf b = input.val[2].f;
    const GLclampf a = input.val[3].f;
    if ((r != this->curClearColorR) || (g != this->curClearColorG) ||
        (b != this->curClearColorB) || (a != this->curClearColorA)) {
        this->curClearColorR = r;
        this->curClearColorG = g;
        this->curClearColorB = b;
        this->curClearColorA = a;
        ::glClearColor(r, g, b, a);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onClearDepth(const State::Vector& input) {
    const GLclampf f = input.val[0].f;
    if (f != this->curClearDepth) {
        this->curClearDepth = f;
        #if ORYOL_OPENGLES2
        ::glClearDepthf(f);
        #else
        ::glClearDepth(f);
        #endif
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onClearStencil(const State::Vector& input) {
    const GLint s = input.val[0].i;
    if (s != this->curClearStencil) {
        this->curClearStencil = s;
        ::glClearStencil(s);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onViewPort(const State::Vector& input) {
    const GLint x        = input.val[0].i;
    const GLint y        = input.val[1].i;
    const GLsizei width  = input.val[2].i;
    const GLsizei height = input.val[3].i;
    if ((x != this->curViewPortX) || (y != this->curViewPortY) ||
        (width != this->curViewPortWidth) || (height != this->curViewPortHeight)) {
        this->curViewPortX = x;
        this->curViewPortY = y;
        this->curViewPortWidth = width;
        this->curViewPortHeight = height;
        ::glViewport(x, y, width, height);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::setupJumpTable() {
    
    // glPolygonOffset(GLfloat, GLfloat)
    this->funcs[State::DepthOffset].cb = &glStateWrapper::onDepthOffset;
    this->funcs[State::DepthOffset].sig = State::F0_F1;
    
    // glScissor(GLint, GLint, GLint, GLint)
    this->funcs[State::ScissorRect].cb = &glStateWrapper::onScissorRect;
    this->funcs[State::ScissorRect].sig = State::I0_I1_I2_I3;
    
    // glBlendColor(GLclampf, GLclampf, GLclampf, GLclampf)
    this->funcs[State::BlendColor].cb = &glStateWrapper::onBlendColor;
    this->funcs[State::BlendColor].sig = State::F0_F1_F2_F3;

    // glClearColor(GLclampf, GLclampf, GLclampf, GLclampf)
    this->funcs[State::ClearColor].cb = &glStateWrapper::onClearColor;
    this->funcs[State::ClearColor].sig = State::F0_F1_F2_F3;

    // glClearDepth(GLclampf)
    this->funcs[State::ClearDepth].cb = &glStateWrapper::onClearDepth;
    this->funcs[State::ClearDepth].sig = State::F0;
    
    // glClearStencil(GLint)
    this->funcs[State::ClearStencil].cb = &glStateWrapper::onClearStencil;
    this->funcs[State::ClearStencil].sig = State::I0;
    
    // glViewPort(GLint, GLint, GLsizei w, GLsizei h)
    this->funcs[State::ViewPort].cb = &glStateWrapper::onViewPort;
    this->funcs[State::ViewPort].sig = State::I0_I1_I2_I3;
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

} // namespace Render
} // namespace Oryol
