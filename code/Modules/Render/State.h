#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::State
    @brief render states
    
    @todo: State description
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {
    
class State {
public:
    /// these are the render state codes (keys)
    enum Code {
        FrontFace,              ///< what's the front side of a poly (CW, CCW)
        CullFaceEnabled,        ///< face-culling enabled (True, False)
        CullFace,               ///< cull face mode (Front, Back, FrontAndBack
        
        DepthOffsetEnabled,     ///< whether depth offset is enabled (True, False)
        DepthOffset,            ///< depth offset values (float factor, float unit)
        
        ScissorTestEnabled,     ///< enable/disable scissor test (True, False)
        ScissorRect,            ///< the scissor rectangle (int left, int bottom, int left, int right)
        
        StencilTestEnabled,     ///< whether stencil testing is enabled (True, False)
        StencilFunc,            ///< the stencil func (func=Never/Always/Less/LessEqual/GreaterEqual/Greater/NotEqual, int ref, uint mask)
        StencilFuncSeparate,    ///< separate stencil funcs for front/back (face=Front/Back/FrontAndBack, func=Never/Always/Less/LessEqual/GreaterEqual/Greater/NotEqual,int ref, uint mask)
        StencilOp,              ///< stencil operation (sfail,dpfail,dppass=Keep/Zero/Replace/Incr/Decr/Invert/IncrWrap/DecrWrap)
        StencilOpSeparate,      ///< separate stencil operations (face=Front/Back/FrontAndBack, sfail,dpfail,dppass=Keep/Zero/Replace/Incr/Decr/Invert/IncrWrap/DecrWrap)
        
        DepthTestEnabled,       ///< whether depth test is enabled (True, False)
        DepthFunc,              ///< depth test function (Never/Always/Less/LessEqual/GreaterEqual/Greater/NotEqual)
        
        BlendEnabled,           ///< whether alpha blending is enabled (True, False)
        BlendEquation,          ///< combined RGB/A blend mode (Add, Subtract, ReverseSubtract)
        BlendEquationSeparate,  ///< separate RGBA + A blend mode (rgb=Add/Subtract/ReverseSubtract, a=Add/Subtract/ReverseSubtract)
        BlendFunc,              ///< RGBA blend function (src,dst=Zero/One/SrcColor/InvSrcColor/DstColor/InvDstColor/SrcAlpha/InvSrcAlpha/DstAlpha/InvDstAlpha/ConstColor/InvConstColor/ConstAlpha/InvConstAlpha)
        BlendFuncSeparate,      ///< separate RGB+A blend func, (srcRGB,dstRGB,srcA,dstA, see above)
        BlendColor,             ///< color for ConstColor/ConstAlpha (4x float)
    
        DitherEnabled,          ///< whether dithering is on (True, False)
        ColorMask,              ///< color channel write mask (4x True|False)
        DepthMask,              ///< whether depth writes are enabled (True, False)
        StencilMask,            ///< stencil write bit mask (uint)
        StencilMaskSeparate,    ///< stencil write bit masks for polygon-side (face=Front|Back|FrontAndBack, uint mask)
        
        ClearColor,             ///< the clear color values (4x float)
        ClearDepth,             ///< the clear depth value (float)
        ClearStencil,           ///< the clear stencil value (int)
        
        ViewPort,               ///< the current viewport (int x, int y, int w, int h)
        DepthRange,             ///< the current depth range (float n, float f)
        
        NumStateCodes,
        InvalidStateCode
    };
    
    /// these are the render state values
    enum Value {
        Undefined,      ///< state is not set
        Zero,           ///< stencil func / blend factor: zero, or replace with zerp
        One,            ///< blend factor: set to one
        
        CW,             ///< for FrontFace state
        CCW,            ///< for FrontFace state
        
        Front,          ///< polygon front side
        Back,           ///< polygon back side
        FrontAndBack,   ///< polygon front and back
        
        Never,          ///< stencil/depth func: never pass
        Always,         ///< stencil/depth func: always pass
        Less,           ///< stencil/depth func: pass if less-then
        LessEqual,      ///< stencil/depth func: pass if less-or-equal
        GreaterEqual,   ///< stencil/depth func: pass if greater-or-equal
        Greater,        ///< stencil/dpeth func: pass if greater
        NotEqual,       ///< stencil/depth func: pass if not equal
        
        Keep,           ///< stencil func: keep value
        // Zero,        ///< stencil func: set value to zero
        Replace,        ///< stencil func: replace with reference value
        Incr,           ///< stencil func: incremenet with saturation
        Decr,           ///< stencil func: decrement with saturation
        Invert,         ///< stencil func: bitwise invert
        IncrWrap,       ///< stencil func: increment with wrap-around
        DecrWrap,       ///< stencil func: decrement with wrap-around
        
        // Zero             ///< blend factor: zero
        // One              ///< blend factor: one
        SrcColor,           ///< blend factor: src-color
        InvSrcColor,        ///< blend factor: one-minus-src-color
        DstColor,           ///< blend factor: dst-color
        InvDstColor,        ///< blend factor: one-minus-dst-color
        SrcAlpha,           ///< blend factor: src-alpha
        InvSrcAlpha,        ///< blend factor: one-minus-src-alpha
        DstAlpha,           ///< blend factor: dst-alpha
        InvDstAlpha,        ///< blend factor: one-minus-dst-alpha
        ConstColor,         ///< blend factor: constant-color
        InvConstColor,      ///< blend factor: one-minus-constant-color
        ConstAlpha,         ///< blend factor: constant-alpha
        InvConstAlpha,      ///< blend factor: one-minus-constant-alpha
        SrcAlphaSaturate,   ///< blend factor: (f,f,f), f=min(As,1-Ad)
        
        NumStateValues,
        InvalidStateValue
    };
    
    /// convert state code to string
    static const char* CodeToString(Code c);
    /// convert string to state code
    static Code CodeFromString(const char* str);
    /// convert state value to string
    static const char* ValueToString(Value v);
    /// convert string to state value
    static Value ValueFromString(const char* str);
};
    
} // namespace Render
} // namespace Oryol