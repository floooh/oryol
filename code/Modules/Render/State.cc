//------------------------------------------------------------------------------
//  State.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "State.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
const char*
State::CodeToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(FrontFace);
        __ORYOL_TOSTRING(CullFaceEnabled);
        __ORYOL_TOSTRING(CullFace);
        __ORYOL_TOSTRING(DepthOffsetEnabled);
        __ORYOL_TOSTRING(DepthOffset);
        __ORYOL_TOSTRING(ScissorTestEnabled);
        __ORYOL_TOSTRING(ScissorRect);
        __ORYOL_TOSTRING(StencilTestEnabled);
        __ORYOL_TOSTRING(StencilFunc);
        __ORYOL_TOSTRING(StencilFuncSeparate);
        __ORYOL_TOSTRING(StencilOp);
        __ORYOL_TOSTRING(StencilOpSeparate);
        __ORYOL_TOSTRING(DepthTestEnabled);
        __ORYOL_TOSTRING(DepthFunc);
        __ORYOL_TOSTRING(BlendEnabled);
        __ORYOL_TOSTRING(BlendEquation);
        __ORYOL_TOSTRING(BlendEquationSeparate);
        __ORYOL_TOSTRING(BlendFunc);
        __ORYOL_TOSTRING(BlendFuncSeparate);
        __ORYOL_TOSTRING(BlendColor);
        __ORYOL_TOSTRING(DitherEnabled);
        __ORYOL_TOSTRING(ColorMask);
        __ORYOL_TOSTRING(DepthMask);
        __ORYOL_TOSTRING(StencilMask);
        __ORYOL_TOSTRING(StencilMaskSeparate);
        __ORYOL_TOSTRING(ClearColor);
        __ORYOL_TOSTRING(ClearDepth);
        __ORYOL_TOSTRING(ClearStencil);
        __ORYOL_TOSTRING(ViewPort);
        __ORYOL_TOSTRING(DepthRange);
        default: return "InvalidStateCode";
    }
}

//------------------------------------------------------------------------------
State::Code
State::CodeFromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(FrontFace);
    __ORYOL_FROMSTRING(CullFaceEnabled);
    __ORYOL_FROMSTRING(CullFace);
    __ORYOL_FROMSTRING(DepthOffsetEnabled);
    __ORYOL_FROMSTRING(DepthOffset);
    __ORYOL_FROMSTRING(ScissorTestEnabled);
    __ORYOL_FROMSTRING(ScissorRect);
    __ORYOL_FROMSTRING(StencilTestEnabled);
    __ORYOL_FROMSTRING(StencilFunc);
    __ORYOL_FROMSTRING(StencilFuncSeparate);
    __ORYOL_FROMSTRING(StencilOp);
    __ORYOL_FROMSTRING(StencilOpSeparate);
    __ORYOL_FROMSTRING(DepthTestEnabled);
    __ORYOL_FROMSTRING(DepthFunc);
    __ORYOL_FROMSTRING(BlendEnabled);
    __ORYOL_FROMSTRING(BlendEquation);
    __ORYOL_FROMSTRING(BlendEquationSeparate);
    __ORYOL_FROMSTRING(BlendFunc);
    __ORYOL_FROMSTRING(BlendFuncSeparate);
    __ORYOL_FROMSTRING(BlendColor);
    __ORYOL_FROMSTRING(DitherEnabled);
    __ORYOL_FROMSTRING(ColorMask);
    __ORYOL_FROMSTRING(DepthMask);
    __ORYOL_FROMSTRING(StencilMask);
    __ORYOL_FROMSTRING(StencilMaskSeparate);
    __ORYOL_FROMSTRING(ClearColor);
    __ORYOL_FROMSTRING(ClearDepth);
    __ORYOL_FROMSTRING(ClearStencil);
    __ORYOL_FROMSTRING(ViewPort);
    __ORYOL_FROMSTRING(DepthRange);
    return InvalidStateCode;
}

//------------------------------------------------------------------------------
const char*
State::ValueToString(Value v) {
    switch (v) {
        __ORYOL_TOSTRING(Undefined);
        __ORYOL_TOSTRING(Zero);
        __ORYOL_TOSTRING(One);
        __ORYOL_TOSTRING(CW);
        __ORYOL_TOSTRING(CCW);
        __ORYOL_TOSTRING(Front);
        __ORYOL_TOSTRING(Back);
        __ORYOL_TOSTRING(FrontAndBack);
        __ORYOL_TOSTRING(Never);
        __ORYOL_TOSTRING(Always);
        __ORYOL_TOSTRING(Less);
        __ORYOL_TOSTRING(LessEqual);
        __ORYOL_TOSTRING(GreaterEqual);
        __ORYOL_TOSTRING(Greater);
        __ORYOL_TOSTRING(NotEqual);
        __ORYOL_TOSTRING(Keep);
        __ORYOL_TOSTRING(Replace);
        __ORYOL_TOSTRING(Incr);
        __ORYOL_TOSTRING(Decr);
        __ORYOL_TOSTRING(Invert);
        __ORYOL_TOSTRING(IncrWrap);
        __ORYOL_TOSTRING(DecrWrap);
        __ORYOL_TOSTRING(SrcColor);
        __ORYOL_TOSTRING(InvSrcColor);
        __ORYOL_TOSTRING(DstColor);
        __ORYOL_TOSTRING(InvDstColor);
        __ORYOL_TOSTRING(SrcAlpha);
        __ORYOL_TOSTRING(InvSrcAlpha);
        __ORYOL_TOSTRING(DstAlpha);
        __ORYOL_TOSTRING(InvDstAlpha);
        __ORYOL_TOSTRING(ConstColor);
        __ORYOL_TOSTRING(InvConstColor);
        __ORYOL_TOSTRING(ConstAlpha);
        __ORYOL_TOSTRING(InvConstAlpha);
        __ORYOL_TOSTRING(SrcAlphaSaturate);
        default: return "InvalidStateValue";
    }
}

//------------------------------------------------------------------------------
State::Value
State::ValueFromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(Undefined);
    __ORYOL_FROMSTRING(Zero);
    __ORYOL_FROMSTRING(One);
    __ORYOL_FROMSTRING(CW);
    __ORYOL_FROMSTRING(CCW);
    __ORYOL_FROMSTRING(Front);
    __ORYOL_FROMSTRING(Back);
    __ORYOL_FROMSTRING(FrontAndBack);
    __ORYOL_FROMSTRING(Never);
    __ORYOL_FROMSTRING(Always);
    __ORYOL_FROMSTRING(Less);
    __ORYOL_FROMSTRING(LessEqual);
    __ORYOL_FROMSTRING(GreaterEqual);
    __ORYOL_FROMSTRING(Greater);
    __ORYOL_FROMSTRING(NotEqual);
    __ORYOL_FROMSTRING(Keep);
    __ORYOL_FROMSTRING(Replace);
    __ORYOL_FROMSTRING(Incr);
    __ORYOL_FROMSTRING(Decr);
    __ORYOL_FROMSTRING(Invert);
    __ORYOL_FROMSTRING(IncrWrap);
    __ORYOL_FROMSTRING(DecrWrap);
    __ORYOL_FROMSTRING(SrcColor);
    __ORYOL_FROMSTRING(InvSrcColor);
    __ORYOL_FROMSTRING(DstColor);
    __ORYOL_FROMSTRING(InvDstColor);
    __ORYOL_FROMSTRING(SrcAlpha);
    __ORYOL_FROMSTRING(InvSrcAlpha);
    __ORYOL_FROMSTRING(DstAlpha);
    __ORYOL_FROMSTRING(InvDstAlpha);
    __ORYOL_FROMSTRING(ConstColor);
    __ORYOL_FROMSTRING(InvConstColor);
    __ORYOL_FROMSTRING(ConstAlpha);
    __ORYOL_FROMSTRING(InvConstAlpha);
    __ORYOL_FROMSTRING(SrcAlphaSaturate);
    return State::InvalidStateValue;
}

} // namespace Render
} // namespace Oryol

