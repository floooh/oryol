//------------------------------------------------------------------------------
//  glStateWrapper.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Render/gl/gl_impl.h"
#include "glStateWrapper.h"
#include "Render/gl/glExt.h"
#include "Core/Memory/Memory.h"
#include "Render/Core/mesh.h"

namespace Oryol {
namespace Render {

using namespace Core;
    
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
curStencilTestEnabled(false),
curDepthTestEnabled(false),
curDepthFunc(GL_LESS),
curBlendEnabled(false),
curBlendEquationRGB(GL_FUNC_ADD),
curBlendEquationAlpha(GL_FUNC_ADD),
curBlendFuncSrcRGB(GL_ONE),
curBlendFuncSrcAlpha(GL_ONE),
curBlendFuncDstRGB(GL_ZERO),
curBlendFuncDstAlpha(GL_ZERO),
curBlendColorR(0.0f),
curBlendColorG(0.0f),
curBlendColorB(0.0f),
curBlendColorA(0.0f),
curDitherEnabled(false),
curColorMaskR(true),
curColorMaskG(true),
curColorMaskB(true),
curColorMaskA(true),
curDepthMask(true),
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
    for (int32 i = 0; i < 2; i++) {
        this->curStencilFunc[i] = GL_ALWAYS;
        this->curStencilFuncRef[i] = 0;
        this->curStencilFuncMask[i] = 0xFFFFFFFF;
        this->curStencilOpSFail[i] = GL_KEEP;
        this->curStencilOpDpFail[i] = GL_KEEP;
        this->curStencilOpDpPass[i] = GL_KEEP;
        this->curStencilMask[i] = 0xFFFFFFFF;
    }

    this->setupStateTranslationTable();
    this->setupStateVector();
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

    /// @todo: this must initialize GL to the default state
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
glStateWrapper::onFrontFace(const ValBlock& input) {
    o_assert_range_dbg(input.val[0].v, State::NumStateValues);
    const GLenum frontFaceMode = this->glEnum[input.val[0].v];
    if (frontFaceMode != this->curFrontFaceMode) {
        this->curFrontFaceMode = frontFaceMode;
        ::glFrontFace(frontFaceMode);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onCullFaceEnabled(const ValBlock& input) {
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
glStateWrapper::onCullFace(const ValBlock& input) {
    o_assert_range_dbg(input.val[0].v, State::NumStateValues);
    const GLenum cullFaceMode = this->glEnum[input.val[0].v];
    if (cullFaceMode != this->curCullFaceMode) {
        this->curCullFaceMode = cullFaceMode;
        ::glCullFace(cullFaceMode);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onDepthOffsetEnabled(const ValBlock& input) {
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
glStateWrapper::onDepthOffset(const ValBlock& input) {
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
glStateWrapper::onScissorTestEnabled(const ValBlock& input) {
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
glStateWrapper::onScissorRect(const ValBlock& input) {
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
glStateWrapper::onStencilTestEnabled(const ValBlock& input) {
    const bool b0 = input.val[0].b;
    if (b0 != this->curStencilTestEnabled) {
        this->curStencilTestEnabled = b0;
        if (b0) {
            ::glEnable(GL_STENCIL_TEST);
        }
        else {
            ::glDisable(GL_STENCIL_TEST);
        }
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onStencilFunc(const ValBlock& input) {
    o_assert_range_dbg(input.val[0].v, State::NumStateValues);
    const GLenum func = this->glEnum[input.val[0].v];
    const GLint ref = input.val[1].i;
    const GLuint mask = (GLuint) input.val[2].i;
    if ((func != this->curStencilFunc[0]) || (func != this->curStencilFunc[1]) ||
        (ref != this->curStencilFuncRef[0]) || (ref != this->curStencilFuncRef[1]) ||
        (mask != this->curStencilFuncMask[0]) || (mask != this->curStencilFuncMask[1])) {
        for (int32 i = 0; i < 2; i++) {
            this->curStencilFunc[i] = func;
            this->curStencilFuncRef[i]  = ref;
            this->curStencilFuncMask[i] = mask;
        }
        ::glStencilFunc(func, ref, mask);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onStencilFuncSeparate(const ValBlock& input) {
    o_assert_range_dbg(input.val[0].v, State::NumStateValues);
    o_assert_range_dbg(input.val[1].v, State::NumStateValues);
    const GLenum face = this->glEnum[input.val[0].v];
    const GLenum func = this->glEnum[input.val[1].v];
    const GLint ref = input.val[2].i;
    const GLuint mask = (GLuint) input.val[3].i;
    int32 i = (GL_FRONT == face) ? 0 : 1;
    if ((func != this->curStencilFunc[i]) || (ref != this->curStencilFuncRef[i]) || (mask != this->curStencilFuncMask[i])) {
        this->curStencilFunc[i] = func;
        this->curStencilFuncRef[i] = ref;
        this->curStencilFuncMask[i] = mask;
        ::glStencilFuncSeparate(face, func, ref, mask);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onStencilOp(const ValBlock& input) {
    o_assert_range_dbg(input.val[0].v, State::NumStateValues);
    o_assert_range_dbg(input.val[1].v, State::NumStateValues);
    o_assert_range_dbg(input.val[2].v, State::NumStateValues);
    const GLenum sfail = this->glEnum[input.val[0].v];
    const GLenum dpfail = this->glEnum[input.val[1].v];
    const GLenum dppass = this->glEnum[input.val[2].v];
    if ((sfail != this->curStencilOpSFail[0]) || (sfail != this->curStencilOpSFail[1]) ||
        (dpfail != this->curStencilOpDpFail[0]) || (dpfail != this->curStencilOpDpFail[1]) ||
        (dppass != this->curStencilOpDpPass[0]) || (dppass != this->curStencilOpDpPass[1])) {
        for (int32 i = 0; i < 2; i++) {
            this->curStencilOpSFail[i] = sfail;
            this->curStencilOpDpFail[i] = dpfail;
            this->curStencilOpDpPass[i] = dppass;
        }
        ::glStencilOp(sfail, dpfail, dppass);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onStencilOpSeparate(const ValBlock& input) {
    o_assert_range_dbg(input.val[0].v, State::NumStateValues);
    o_assert_range_dbg(input.val[1].v, State::NumStateValues);
    o_assert_range_dbg(input.val[2].v, State::NumStateValues);
    o_assert_range_dbg(input.val[3].v, State::NumStateValues);
    const GLenum face = this->glEnum[input.val[0].v];
    const GLenum sfail = this->glEnum[input.val[1].v];
    const GLenum dpfail = this->glEnum[input.val[2].v];
    const GLenum dppass = this->glEnum[input.val[3].v];
    int32 i = (GL_FRONT == face) ? 0 : 1;
    if ((sfail != this->curStencilOpSFail[i]) || (dpfail != this->curStencilOpDpFail[i]) || (dppass != this->curStencilOpDpPass[i])) {
        this->curStencilOpSFail[i] = sfail;
        this->curStencilOpDpFail[i] = dpfail;
        this->curStencilOpDpPass[i] = dppass;
        ::glStencilOpSeparate(face, sfail, dpfail, dppass);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onDepthTestEnabled(const ValBlock& input) {
    const bool b0 = input.val[0].b;
    if (b0 != this->curDepthTestEnabled) {
        this->curDepthTestEnabled = b0;
        if (b0) {
            ::glEnable(GL_DEPTH_TEST);
        }
        else {
            ::glDisable(GL_DEPTH_TEST);
        }
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onDepthFunc(const ValBlock& input) {
    o_assert_range_dbg(input.val[0].v, State::NumStateValues);
    const GLenum depthFunc = this->glEnum[input.val[0].v];
    if (depthFunc != this->curDepthFunc) {
        this->curDepthFunc = depthFunc;
        ::glDepthFunc(depthFunc);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onBlendEnabled(const ValBlock& input) {
    const bool b0 = input.val[0].b;
    if (b0 != this->curBlendEnabled) {
        this->curBlendEnabled = b0;
        if (b0) {
            ::glEnable(GL_BLEND);
        }
        else {
            ::glDisable(GL_BLEND);
        }
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onBlendEquation(const ValBlock& input) {
    o_assert_range_dbg(input.val[0].v, State::NumStateValues);
    const GLenum mode = this->glEnum[input.val[0].v];
    if ((mode != this->curBlendEquationRGB) || (mode != this->curBlendEquationAlpha)) {
        this->curBlendEquationRGB = mode;
        this->curBlendEquationAlpha = mode;
        ::glBlendEquation(mode);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onBlendEquationSeparate(const ValBlock& input) {
    o_assert_range_dbg(input.val[0].v, State::NumStateValues);
    o_assert_range_dbg(input.val[1].v, State::NumStateValues);
    const GLenum modeRGB = this->glEnum[input.val[0].v];
    const GLenum modeAlpha = this->glEnum[input.val[1].v];
    if ((modeRGB != this->curBlendEquationRGB) || (modeAlpha != this->curBlendEquationAlpha)) {
        this->curBlendEquationRGB = modeRGB;
        this->curBlendEquationAlpha = modeAlpha;
        ::glBlendEquationSeparate(modeRGB, modeAlpha);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onBlendFunc(const ValBlock& input) {
    o_assert_range_dbg(input.val[0].v, State::NumStateValues);
    o_assert_range_dbg(input.val[1].v, State::NumStateValues);
    const GLenum src = this->glEnum[input.val[0].v];
    const GLenum dst = this->glEnum[input.val[1].v];
    if ((src != this->curBlendFuncSrcRGB) || (src != this->curBlendFuncSrcAlpha) ||
        (dst != this->curBlendFuncDstRGB) || (dst != this->curBlendFuncDstAlpha)) {
        this->curBlendFuncSrcRGB = this->curBlendFuncSrcAlpha = src;
        this->curBlendFuncDstRGB = this->curBlendFuncDstAlpha = dst;
        ::glBlendFunc(src, dst);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onBlendFuncSeparate(const ValBlock& input) {
    o_assert_range_dbg(input.val[0].v, State::NumStateValues);
    o_assert_range_dbg(input.val[1].v, State::NumStateValues);
    o_assert_range_dbg(input.val[2].v, State::NumStateValues);
    o_assert_range_dbg(input.val[3].v, State::NumStateValues);
    const GLenum srcRGB = this->glEnum[input.val[0].v];
    const GLenum dstRGB = this->glEnum[input.val[1].v];
    const GLenum srcAlpha = this->glEnum[input.val[2].v];
    const GLenum dstAlpha = this->glEnum[input.val[3].v];
    if ((srcRGB != this->curBlendFuncSrcRGB) || (srcAlpha != this->curBlendFuncSrcAlpha) ||
        (dstRGB != this->curBlendFuncDstRGB) || (dstAlpha != this->curBlendFuncDstAlpha)) {
        this->curBlendFuncSrcRGB = srcRGB;
        this->curBlendFuncSrcAlpha = srcAlpha;
        this->curBlendFuncDstRGB = dstRGB;
        this->curBlendFuncDstAlpha = dstAlpha;
        ::glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onBlendColor(const ValBlock& input) {
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
glStateWrapper::onDitherEnabled(const ValBlock& input) {
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
glStateWrapper::onColorMask(const ValBlock& input) {
    const bool br = input.val[0].b;
    const bool bg = input.val[1].b;
    const bool bb = input.val[2].b;
    const bool ba = input.val[3].b;
    if ((br != this->curColorMaskR) || (bg != this->curColorMaskG) || (bb != this->curColorMaskB) || (ba != this->curColorMaskA)) {
        this->curColorMaskR = br;
        this->curColorMaskG = bg;
        this->curColorMaskB = bb;
        this->curColorMaskA = ba;
        ::glColorMask(br, bg, bb, ba);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onDepthMask(const ValBlock& input) {
    const bool b0 = input.val[0].b;
    if (b0 != this->curDepthMask) {
        this->curDepthMask = b0;
        ::glDepthMask(b0);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onStencilMask(const ValBlock& input) {
    const GLuint mask = (GLuint) input.val[0].i;
    if ((mask != this->curStencilMask[0]) || (mask != this->curStencilMask[1])) {
        this->curStencilMask[0] = mask;
        this->curStencilMask[1] = mask;
        ::glStencilMask(mask);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onStencilMaskSeparate(const ValBlock& input) {
    o_assert_range_dbg(input.val[0].v, State::NumStateValues);
    const GLenum face = this->glEnum[input.val[0].v];
    const GLuint mask = input.val[1].i;
    int32 i = (face == GL_FRONT) ? 0 : 1;
    if (mask != this->curStencilMask[i]) {
        this->curStencilMask[i] = mask;
        ::glStencilMaskSeparate(face, mask);
    }
    
}

//------------------------------------------------------------------------------
void
glStateWrapper::onClearColor(const ValBlock& input) {
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
glStateWrapper::onClearDepth(const ValBlock& input) {
    const GLclampf f = input.val[0].f;
    if (f != this->curClearDepth) {
        this->curClearDepth = f;
        ::glClearDepth(f);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onClearStencil(const ValBlock& input) {
    const GLint s = input.val[0].i;
    if (s != this->curClearStencil) {
        this->curClearStencil = s;
        ::glClearStencil(s);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::onViewPort(const ValBlock& input) {
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
glStateWrapper::onDepthRange(const ValBlock& input) {
    const GLfloat nearVal = input.val[0].f;
    const GLfloat farVal = input.val[1].f;
    if ((nearVal != this->curDepthRangeNear) || (farVal != this->curDepthRangeFar)) {
        this->curDepthRangeNear = nearVal;
        this->curDepthRangeFar = farVal;
        ::glDepthRange(nearVal, farVal);
    }
}

//------------------------------------------------------------------------------
void
glStateWrapper::setupStateTranslationTable() {
    #if ORYOL_DEBUG
        Memory::Clear(this->glEnum, sizeof(this->glEnum));
    #endif
    this->glEnum[State::Undefined]          = -1;
    this->glEnum[State::Zero]               = GL_ZERO;
    this->glEnum[State::One]                = GL_ONE;
    this->glEnum[State::CW]                 = GL_CW;
    this->glEnum[State::CCW]                = GL_CCW;
    this->glEnum[State::Front]              = GL_FRONT;
    this->glEnum[State::Back]               = GL_BACK;
    this->glEnum[State::FrontAndBack]       = GL_FRONT_AND_BACK;
    this->glEnum[State::Never]              = GL_NEVER;
    this->glEnum[State::Always]             = GL_ALWAYS;
    this->glEnum[State::Less]               = GL_LESS;
    this->glEnum[State::LessEqual]          = GL_LEQUAL;
    this->glEnum[State::GreaterEqual]       = GL_GEQUAL;
    this->glEnum[State::Greater]            = GL_GREATER;
    this->glEnum[State::NotEqual]           = GL_NOTEQUAL;
    this->glEnum[State::Keep]               = GL_KEEP;
    this->glEnum[State::Replace]            = GL_REPLACE;
    this->glEnum[State::Incr]               = GL_INCR;
    this->glEnum[State::Decr]               = GL_DECR;
    this->glEnum[State::Invert]             = GL_INVERT;
    this->glEnum[State::IncrWrap]           = GL_INCR_WRAP;
    this->glEnum[State::DecrWrap]           = GL_DECR_WRAP;
    this->glEnum[State::SrcColor]           = GL_SRC_COLOR;
    this->glEnum[State::InvSrcColor]        = GL_ONE_MINUS_SRC_COLOR;
    this->glEnum[State::DstColor]           = GL_DST_COLOR;
    this->glEnum[State::InvDstColor]        = GL_ONE_MINUS_DST_COLOR;
    this->glEnum[State::SrcAlpha]           = GL_SRC_ALPHA;
    this->glEnum[State::InvSrcAlpha]        = GL_ONE_MINUS_SRC_ALPHA;
    this->glEnum[State::DstAlpha]           = GL_DST_ALPHA;
    this->glEnum[State::InvDstAlpha]        = GL_ONE_MINUS_DST_ALPHA;
    this->glEnum[State::ConstColor]         = GL_CONSTANT_COLOR;
    this->glEnum[State::InvConstColor]      = GL_ONE_MINUS_CONSTANT_COLOR;
    this->glEnum[State::ConstAlpha]         = GL_CONSTANT_ALPHA;
    this->glEnum[State::InvConstAlpha]      = GL_ONE_MINUS_CONSTANT_ALPHA;
    this->glEnum[State::SrcAlphaSaturate]   = GL_SRC_ALPHA_SATURATE;

    // check that we didn't forget some state
    #if ORYOL_DEBUG
    for (int32 i = 0; i < State::NumStateValues; i++) {
        if (State::Zero != i) {
            o_assert(0 != this->glEnum[i]);
        }
    }
    #endif
}

//------------------------------------------------------------------------------
void
glStateWrapper::setupStateVector() {
    
    // glFrontFace(GLenum dir)
    this->funcs[State::FrontFace].cb  = &glStateWrapper::onFrontFace;
    this->funcs[State::FrontFace].sig = V0;
    
    // glEnable(GL_CULL_FACE)
    this->funcs[State::CullFaceEnabled].cb = &glStateWrapper::onCullFaceEnabled;
    this->funcs[State::CullFaceEnabled].sig = V0;
    
    // glCullFace(GLenum mode)
    this->funcs[State::CullFace].cb = &glStateWrapper::onCullFace;
    this->funcs[State::CullFace].sig = V0;
    
    // glEnable(GL_POLYGON_OFFSET_FILL)
    this->funcs[State::DepthOffsetEnabled].cb = &glStateWrapper::onDepthOffsetEnabled;
    this->funcs[State::DepthOffsetEnabled].sig = V0;
    
    // glPolygonOffset(GLfloat, GLfloat)
    this->funcs[State::DepthOffset].cb = &glStateWrapper::onDepthOffset;
    this->funcs[State::DepthOffset].sig = F0_F1;
    
    // glEnable(GL_SCISSOR_TEST)
    this->funcs[State::ScissorTestEnabled].cb = &glStateWrapper::onScissorTestEnabled;
    this->funcs[State::ScissorTestEnabled].sig = V0;
    
    // glScissor(GLint, GLint, GLint, GLint)
    this->funcs[State::ScissorRect].cb = &glStateWrapper::onScissorRect;
    this->funcs[State::ScissorRect].sig = I0_I1_I2_I3;
    
    // glEnable(GL_STENCIL_TEST)
    this->funcs[State::StencilTestEnabled].cb = &glStateWrapper::onStencilTestEnabled;
    this->funcs[State::StencilTestEnabled].sig = V0;
    
    // glStencilFunc(GLenum, int, uint)
    this->funcs[State::StencilFunc].cb = &glStateWrapper::onStencilFunc;
    this->funcs[State::StencilFunc].sig  = V0_I0_I1;
    
    // glStencilFuncSeparate(GLenum, GLenum, int, uint)
    this->funcs[State::StencilFuncSeparate].cb = &glStateWrapper::onStencilFuncSeparate;
    this->funcs[State::StencilFuncSeparate].sig = V0_V1_I0_I1;
    
    // glStencilOp(GLenum, GLenum, GLenum)
    this->funcs[State::StencilOp].cb = &glStateWrapper::onStencilOp;
    this->funcs[State::StencilOp].sig = V0_V1_V2;
    
    // glStencilOpSeparate(GLenum, GLenum, GLenum, GLenum)
    this->funcs[State::StencilOpSeparate].cb = &glStateWrapper::onStencilOpSeparate;
    this->funcs[State::StencilOpSeparate].sig  = V0_V1_V2_V3;

    // glEnable(DEPTH_TEST)
    this->funcs[State::DepthTestEnabled].cb = &glStateWrapper::onDepthTestEnabled;
    this->funcs[State::DepthTestEnabled].sig = B0;
    
    // glDepthFunc(GLenum)
    this->funcs[State::DepthFunc].cb = &glStateWrapper::onDepthFunc;
    this->funcs[State::DepthFunc].sig = V0;
    
    // glEnable(GL_BLEND)
    this->funcs[State::BlendEnabled].cb = &glStateWrapper::onBlendEnabled;
    this->funcs[State::BlendEnabled].sig = V0;
    
    // glBlendEquation(GLenum)
    this->funcs[State::BlendEquation].cb = &glStateWrapper::onBlendEquation;
    this->funcs[State::BlendEquation].sig = V0;
    
    // glBlendEquationSeparate(GLenum, GLenum)
    this->funcs[State::BlendEquationSeparate].cb = &glStateWrapper::onBlendEquationSeparate;
    this->funcs[State::BlendEquationSeparate].sig = V0_V1;
    
    // glBlendFunc(GLenum, GLenum)
    this->funcs[State::BlendFunc].cb = &glStateWrapper::onBlendFunc;
    this->funcs[State::BlendFunc].sig = V0_V1;
    
    // glBlendFuncSeparate(GLenum, GLenum, GLenum, GLenum)
    this->funcs[State::BlendFuncSeparate].cb = &glStateWrapper::onBlendFuncSeparate;
    this->funcs[State::BlendFuncSeparate].sig = V0_V1_V2_V3;
    
    // glBlendColor(GLclampf, GLclampf, GLclampf, GLclampf)
    this->funcs[State::BlendColor].cb = &glStateWrapper::onBlendColor;
    this->funcs[State::BlendColor].sig = F0_F1_F2_F3;

    // glEnable(GL_DITHER)
    this->funcs[State::DitherEnabled].cb = &glStateWrapper::onDitherEnabled;
    this->funcs[State::DitherEnabled].sig = V0;
    
    // glColorMask(GLenum, GLenum, GLenum, GLenum)
    this->funcs[State::ColorMask].cb = &glStateWrapper::onColorMask;
    this->funcs[State::ColorMask].sig = V0_V1_V2_V3;
    
    // glDepthMask(GLenum)
    this->funcs[State::DepthMask].cb = &glStateWrapper::onDepthMask;
    this->funcs[State::DepthMask].sig = B0;
    
    // glStencilMask(GLuint)
    this->funcs[State::StencilMask].cb = &glStateWrapper::onStencilMask;
    this->funcs[State::StencilMask].sig = I0;
    
    // glStencilMaskSeparate(GLenum, GLuint)
    this->funcs[State::StencilMaskSeparate].cb = &glStateWrapper::onStencilMaskSeparate;
    this->funcs[State::StencilMaskSeparate].sig = V0_I0;
    
    // glClearColor(GLclampf, GLclampf, GLclampf, GLclampf)
    this->funcs[State::ClearColor].cb = &glStateWrapper::onClearColor;
    this->funcs[State::ClearColor].sig = F0_F1_F2_F3;

    // glClearDepth(GLclampf)
    this->funcs[State::ClearDepth].cb = &glStateWrapper::onClearDepth;
    this->funcs[State::ClearDepth].sig = F0;
    
    // glClearStencil(GLint)
    this->funcs[State::ClearStencil].cb = &glStateWrapper::onClearStencil;
    this->funcs[State::ClearStencil].sig = I0;
    
    // glViewPort(GLint, GLint, GLsizei w, GLsizei h)
    this->funcs[State::ViewPort].cb = &glStateWrapper::onViewPort;
    this->funcs[State::ViewPort].sig = I0_I1_I2_I3;
    
    // glDepthRange(GLclampf, GLclampf)
    this->funcs[State::DepthRange].cb = &glStateWrapper::onDepthRange;
    this->funcs[State::DepthRange].sig = F0_F1;
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
glStateWrapper::BindMesh(const mesh* msh) {
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

} // namespace Render
} // namespace Oryol
