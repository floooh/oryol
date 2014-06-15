//------------------------------------------------------------------------------
//  glStateWrapper.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Render/gl/gl_impl.h"
#include "glStateWrapper.h"
#include "Render/gl/glExt.h"
#include "Core/Memory/Memory.h"
#include "Render/Core/mesh.h"
#include "Render/Core/stateBlock.h"
#include "Render/Core/programBundle.h"
#include "Render/Core/depthStencilState.h"
#include "Render/Core/blendState.h"

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

//------------------------------------------------------------------------------
glStateWrapper::glStateWrapper() :
isValid(false),
curFrontFaceMode(GL_CCW),
curCullFaceEnabled(false),
curCullFaceMode(GL_BACK),
curDepthOffsetEnabled(false),
curDepthOffsetFactor(0.0f),
curDepthOffsetUnits(0.0f),
curScissorTestEnabled(false),
curScissorLeft(0),
curScissorBottom(0),
curScissorWidth(-1),
curScissorHeight(-1),
curBlendColorR(0.0f),
curBlendColorG(0.0f),
curBlendColorB(0.0f),
curBlendColorA(0.0f),
curDitherEnabled(false),
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

    this->setupDepthStencilState();
    this->setupBlendState();
}

//------------------------------------------------------------------------------
void
glStateWrapper::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
glStateWrapper::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
glStateWrapper::setupDepthStencilState() {
    this->curDepthStencilState.depthCompareFunc = CompareFunc::Always;
    this->curDepthStencilState.depthWriteEnabled = false;
    this->curDepthStencilState.stencilTestEnabled = false;
    for (int32 i = 0; i < Face::NumSides; i++) {
        this->curDepthStencilState.stencilState[i].stencilFailOp      = StencilOp::Keep;
        this->curDepthStencilState.stencilState[i].depthFailOp        = StencilOp::Keep;
        this->curDepthStencilState.stencilState[i].depthStencilPassOp = StencilOp::Keep;
        this->curDepthStencilState.stencilState[i].stencilCompareFunc = CompareFunc::Always;
        this->curDepthStencilState.stencilState[i].stencilReadMask    = 0xFFFFFFFF;
        this->curDepthStencilState.stencilState[i].stencilWriteMask   = 0xFFFFFFFF;
        this->curDepthStencilState.stencilState[i].stencilRef         = 0;
    }
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
glStateWrapper::applyStencilState(const depthStencilState* dds, Face::Code face, GLenum glFace) {

    const DepthStencilStateSetup& setup = dds->GetSetup();

    const CompareFunc::Code cmpFunc = setup.GetStencilCompareFunc(face);
    o_assert_range_dbg(cmpFunc, CompareFunc::NumCompareFuncs);
    const uint32 readMask = setup.GetStencilReadMask(face);
    const int32 stencilRef = setup.GetStencilRef(face);
    if ((cmpFunc != this->curDepthStencilState.stencilState[face].stencilCompareFunc) ||
        (readMask != this->curDepthStencilState.stencilState[face].stencilReadMask) ||
        (stencilRef != this->curDepthStencilState.stencilState[face].stencilRef))
    {
        this->curDepthStencilState.stencilState[face].stencilCompareFunc = cmpFunc;
        this->curDepthStencilState.stencilState[face].stencilReadMask = readMask;
        this->curDepthStencilState.stencilState[face].stencilRef = stencilRef;
        ::glStencilFuncSeparate(glFace, mapCompareFunc[cmpFunc], stencilRef, readMask);
        ORYOL_GL_CHECK_ERROR();
    }
    
    const StencilOp::Code sFailOp = setup.GetStencilFailOp(face);
    o_assert_range_dbg(sFailOp, StencilOp::NumStencilOperations);
    const StencilOp::Code dFailOp = setup.GetDepthFailOp(face);
    o_assert_range_dbg(dFailOp, StencilOp::NumStencilOperations);
    const StencilOp::Code passOp = setup.GetDepthStencilPassOp(face);
    o_assert_range_dbg(passOp, StencilOp::NumStencilOperations);
    if ((sFailOp != this->curDepthStencilState.stencilState[face].stencilFailOp) ||
        (dFailOp != this->curDepthStencilState.stencilState[face].depthFailOp) ||
        (passOp  != this->curDepthStencilState.stencilState[face].depthStencilPassOp)) {
        
        this->curDepthStencilState.stencilState[face].stencilFailOp = sFailOp;
        this->curDepthStencilState.stencilState[face].depthFailOp = sFailOp;
        this->curDepthStencilState.stencilState[face].depthStencilPassOp = passOp;
        ::glStencilOpSeparate(GL_FRONT, mapStencilOp[sFailOp], mapStencilOp[dFailOp], mapStencilOp[passOp]);
        ORYOL_GL_CHECK_ERROR();
    }
    
    const uint32 writeMask = setup.GetStencilWriteMask(face);
    if (writeMask != this->curDepthStencilState.stencilState[face].stencilWriteMask) {
        this->curDepthStencilState.stencilState[face].stencilWriteMask = writeMask;
        ::glStencilMask(writeMask);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::ApplyDepthStencilState(const depthStencilState* dds) {
    o_assert_dbg((nullptr != dds) && (dds->GetState() == Resource::State::Valid));
    const DepthStencilStateSetup& setup = dds->GetSetup();

    const CompareFunc::Code depthCmpFunc = setup.GetDepthCompareFunc();
    o_assert_range_dbg(depthCmpFunc, CompareFunc::NumCompareFuncs);
    if (depthCmpFunc != this->curDepthStencilState.depthCompareFunc) {
        this->curDepthStencilState.depthCompareFunc = depthCmpFunc;
        ::glDepthFunc(mapCompareFunc[depthCmpFunc]);
        ORYOL_GL_CHECK_ERROR();
    }

    const bool depthWriteEnabled = setup.GetDepthWriteEnabled();
    if (depthWriteEnabled != this->curDepthStencilState.depthWriteEnabled) {
        this->curDepthStencilState.depthWriteEnabled = depthWriteEnabled;
        ::glDepthMask(depthWriteEnabled);
        ORYOL_GL_CHECK_ERROR();
    }
    
    const bool stencilTestEnabled = setup.GetStencilTestEnabled();
    if (stencilTestEnabled != this->curDepthStencilState.stencilTestEnabled) {
        this->curDepthStencilState.stencilTestEnabled = stencilTestEnabled;
        if (stencilTestEnabled) {
            ::glEnable(GL_STENCIL_TEST);
        }
        else {
            ::glDisable(GL_STENCIL_TEST);
        }
        ORYOL_GL_CHECK_ERROR();
    }
    
    this->applyStencilState(dds, Face::Front, GL_FRONT);
    this->applyStencilState(dds, Face::Back, GL_BACK);
}

//------------------------------------------------------------------------------
void
glStateWrapper::setupBlendState() {
    this->curBlendState.blendingEnabled = false;
    this->curBlendState.rgbSrcFactor = BlendFactor::One;
    this->curBlendState.rgbDstFactor = BlendFactor::Zero;
    this->curBlendState.rgbBlendOperation = BlendOperation::Add;
    this->curBlendState.alphaSrcFactor = BlendFactor::One;
    this->curBlendState.alphaDstFactor = BlendFactor::Zero;
    this->curBlendState.alphaBlendOperation = BlendOperation::Add;
    this->curBlendState.colorWriteMask = ColorWriteMask::All;
    
    ::glDisable(GL_BLEND);
    ::glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
    ::glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    ::glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glStateWrapper::ApplyBlendState(const blendState* bs) {
    o_assert_dbg((nullptr != bs) && (bs->GetState() == Resource::State::Valid));
    const BlendStateSetup& setup = bs->GetSetup();
    
    const bool enabled = setup.GetBlendingEnabled();
    if (enabled != this->curBlendState.blendingEnabled) {
        this->curBlendState.blendingEnabled = enabled;
        if (enabled) {
            ::glEnable(GL_BLEND);
        }
        else {
            ::glDisable(GL_BLEND);
        }
        ORYOL_GL_CHECK_ERROR();
    }
    const BlendFactor::Code srcRgbFactor = setup.GetSrcFactorRGB();
    const BlendFactor::Code dstRgbFactor = setup.GetDstFactorRGB();
    const BlendFactor::Code srcAlphaFactor = setup.GetSrcFactorAlpha();
    const BlendFactor::Code dstAlphaFactor = setup.GetDstFactorAlpha();
    if ((srcRgbFactor != this->curBlendState.rgbSrcFactor) ||
        (dstRgbFactor != this->curBlendState.rgbDstFactor) ||
        (srcAlphaFactor != this->curBlendState.alphaSrcFactor) ||
        (dstAlphaFactor != this->curBlendState.alphaDstFactor)) {
        
        this->curBlendState.rgbSrcFactor = srcRgbFactor;
        this->curBlendState.rgbDstFactor = dstRgbFactor;
        this->curBlendState.alphaSrcFactor = srcAlphaFactor;
        this->curBlendState.alphaDstFactor = dstAlphaFactor;
        
        ::glBlendFuncSeparate(mapBlendFactor[srcRgbFactor],
                              mapBlendFactor[dstRgbFactor],
                              mapBlendFactor[srcAlphaFactor],
                              mapBlendFactor[dstAlphaFactor]);
        ORYOL_GL_CHECK_ERROR();
    }
    const BlendOperation::Code rgbOp = setup.GetOpRGB();
    const BlendOperation::Code alphaOp = setup.GetOpAlpha();
    if ((rgbOp != this->curBlendState.rgbBlendOperation) ||
        (alphaOp != this->curBlendState.alphaBlendOperation)) {
        
        this->curBlendState.rgbBlendOperation = rgbOp;
        this->curBlendState.alphaBlendOperation = alphaOp;
        
        ::glBlendEquationSeparate(mapBlendOp[rgbOp], mapBlendOp[alphaOp]);
        ORYOL_GL_CHECK_ERROR();
    }
    
    const ColorWriteMask::Code colorMask = setup.GetColorWriteMask();
    if (colorMask != this->curBlendState.colorWriteMask) {
        ::glColorMask((colorMask & ColorWriteMask::R) != 0,
                      (colorMask & ColorWriteMask::G) != 0,
                      (colorMask & ColorWriteMask::B) != 0,
                      (colorMask & ColorWriteMask::A) != 0);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onFrontFace(const State::Vector& input) {
    const GLenum frontFaceMode = input.val[0].v;
    if (frontFaceMode != this->curFrontFaceMode) {
        this->curFrontFaceMode = frontFaceMode;
        ::glFrontFace(frontFaceMode);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onCullFaceEnabled(const State::Vector& input) {
    const bool b0 = input.val[0].b;
    if (b0 != this->curCullFaceEnabled) {
        this->curCullFaceEnabled = b0;
        if (b0) {
            ::glEnable(GL_CULL_FACE);
        }
        else {
            ::glDisable(GL_CULL_FACE);
        }
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onCullFace(const State::Vector& input) {
    const GLenum cullFaceMode = input.val[0].v;
    if (cullFaceMode != this->curCullFaceMode) {
        this->curCullFaceMode = cullFaceMode;
        ::glCullFace(cullFaceMode);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onDepthOffsetEnabled(const State::Vector& input) {
    const bool b0 = input.val[0].b;
    if (b0 != this->curDepthOffsetEnabled) {
        this->curDepthOffsetEnabled = b0;
        if (b0) {
            ::glEnable(GL_POLYGON_OFFSET_FILL);
        }
        else {
            ::glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }
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
glStateWrapper::onScissorTestEnabled(const State::Vector& input) {
    const bool b0 = input.val[0].b;
    if (b0 != this->curScissorTestEnabled) {
        this->curScissorTestEnabled = b0;
        if (b0) {
            ::glEnable(GL_SCISSOR_TEST);
        }
        else {
            ::glDisable(GL_SCISSOR_TEST);
        }
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
glStateWrapper::onDitherEnabled(const State::Vector& input) {
    const bool b0 = input.val[0].b;
    if (b0 != this->curDitherEnabled) {
        this->curDitherEnabled = b0;
        if (b0) {
            ::glEnable(GL_DITHER);
        }
        else {
            ::glDisable(GL_DITHER);
        }
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
    
    // glFrontFace(GLenum dir)
    this->funcs[State::FrontFace].cb  = &glStateWrapper::onFrontFace;
    this->funcs[State::FrontFace].sig = State::V0;
    
    // glEnable(GL_CULL_FACE)
    this->funcs[State::CullFaceEnabled].cb = &glStateWrapper::onCullFaceEnabled;
    this->funcs[State::CullFaceEnabled].sig = State::B0;
    
    // glCullFace(GLenum mode)
    this->funcs[State::CullFace].cb = &glStateWrapper::onCullFace;
    this->funcs[State::CullFace].sig = State::V0;
    
    // glEnable(GL_POLYGON_OFFSET_FILL)
    this->funcs[State::DepthOffsetEnabled].cb = &glStateWrapper::onDepthOffsetEnabled;
    this->funcs[State::DepthOffsetEnabled].sig = State::B0;
    
    // glPolygonOffset(GLfloat, GLfloat)
    this->funcs[State::DepthOffset].cb = &glStateWrapper::onDepthOffset;
    this->funcs[State::DepthOffset].sig = State::F0_F1;
    
    // glEnable(GL_SCISSOR_TEST)
    this->funcs[State::ScissorTestEnabled].cb = &glStateWrapper::onScissorTestEnabled;
    this->funcs[State::ScissorTestEnabled].sig = State::B0;
    
    // glScissor(GLint, GLint, GLint, GLint)
    this->funcs[State::ScissorRect].cb = &glStateWrapper::onScissorRect;
    this->funcs[State::ScissorRect].sig = State::I0_I1_I2_I3;
    
    // glBlendColor(GLclampf, GLclampf, GLclampf, GLclampf)
    this->funcs[State::BlendColor].cb = &glStateWrapper::onBlendColor;
    this->funcs[State::BlendColor].sig = State::F0_F1_F2_F3;

    // glEnable(GL_DITHER)
    this->funcs[State::DitherEnabled].cb = &glStateWrapper::onDitherEnabled;
    this->funcs[State::DitherEnabled].sig = State::B0;
    
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
        this->curVertexArrayObject = vao;
        glExt::BindVertexArray(vao);
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::BindProgram(const programBundle* progBundle) {
    // bind the program bundle
    if (nullptr == progBundle) {
        this->InvalidateProgramState();
    }
    else {
        GLuint glProg = progBundle->getProgram();
        o_assert_dbg(0 != glProg);
        this->UseProgram(glProg);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::BindMesh(const mesh* msh, const programBundle* progBundle) {
    
    // bind the mesh
    if (nullptr == msh) {
        this->InvalidateMeshState();
    }
    else {
        #if ORYOL_USE_GLGETATTRIBLOCATION
            // This is the code path which uses glGetAttribLocation instead of
            // glBindAttribLocation, which must be used if GL_MAX_VERTEX_ATTRIBS is smaller
            // then VertexAttr::NumVertexAttrs. The only currently known platform
            // where this applies is the Raspberry Pi where GL_MAX_VERTEX_ATTRIBS==8
            o_assert_dbg(progBundle->getProgram() == this->curProgram);
            const GLuint vb = msh->glGetVertexBuffer();
            const GLuint ib = msh->glGetIndexBuffer();
            this->BindIndexBuffer(ib);
            if (vb != this->curVertexBuffer) {
                this->curVertexBuffer = vb;
                ::glBindBuffer(GL_ARRAY_BUFFER, vb);
                ORYOL_GL_CHECK_ERROR();
                int32 maxUsedAttrib = 0;
                for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
                    const glVertexAttr& attr = msh->glAttr(i);
                    const GLint glAttribIndex = progBundle->getAttribLocation((VertexAttr::Code)i);
                    if (-1 != glAttribIndex) {
                        ::glVertexAttribPointer(glAttribIndex, attr.size, attr.type, attr.normalized, attr.stride, (const GLvoid*) (GLintptr) attr.offset);
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
            }
        #else
            if (glExt::HasExtension(glExt::VertexArrayObject)) {
                GLuint vao = msh->glGetVertexArrayObject();
                o_assert_dbg(0 != vao);
                this->BindVertexArrayObject(vao);
            }
            else {
                const GLuint vb = msh->glGetVertexBuffer();
                const GLuint ib = msh->glGetIndexBuffer();
                this->BindIndexBuffer(ib);
                if (vb != this->curVertexBuffer) {
                    this->curVertexBuffer = vb;
                    ::glBindBuffer(GL_ARRAY_BUFFER, vb);
                    ORYOL_GL_CHECK_ERROR();
                    for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
                        const glVertexAttr& attr = msh->glAttr(i);
                        if (attr.enabled) {
                            ::glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, attr.stride, (const GLvoid*) (GLintptr) attr.offset);
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
            }
        #endif
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
glStateWrapper::ApplyStateBlock(const stateBlock* sb) {
    o_assert_dbg((nullptr != sb) && (sb->GetState() == Resource::State::Valid));
    int32 numStates = sb->GetNumStates();
    const State::Object* states = sb->GetStates();
    for (int32 i = 0; i < numStates; i++) {
        const State::Object& curState = states[i];
        o_assert_dbg((curState.state >= 0) && (curState.state < State::NumStateCodes));
        o_assert_dbg(curState.sig == this->funcs[curState.state].sig);
        (this->*funcs[curState.state].cb)(curState.vec);
    }
}

} // namespace Render
} // namespace Oryol
