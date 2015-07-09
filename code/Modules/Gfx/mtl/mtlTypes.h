#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTypes
    @ingroup _priv
    @brief Oryol to Metal type mapping
    
    NOTE: only include this file from a Objective-C(++) file!
*/
#include "Gfx/Core/Enums.h"

#if !defined(__OBJC__)
#error "mtlTypes.h: Must be included from Obj-C source!"
#endif

namespace Oryol {
namespace _priv {

class mtlTypes {
public:
    /// convert render target pixel format
    static MTLPixelFormat asRenderTargetFormat(PixelFormat::Code fmt);
    /// convert usage to MTLResourceOptions bit mask
    static MTLResourceOptions asBufferResourceOptions(Usage::Code usage);
    /// convert compare function enum value
    static MTLCompareFunction asCompareFunc(CompareFunc::Code cmp);
    /// convert stencil operation enum value
    static MTLStencilOperation asStencilOp(StencilOp::Code op);
    /// convert color-write-mask
    static MTLColorWriteMask asColorWriteMask(PixelChannel::Mask mask);
    /// convert blend operation
    static MTLBlendOperation asBlendOp(BlendOperation::Code op);
    /// convert blend factor
    static MTLBlendFactor asBlendFactor(BlendFactor::Code factor);
    /// convert vertex format
    static MTLVertexFormat asVertexFormat(VertexFormat::Code fmt);
    /// convert vertex step function
    static MTLVertexStepFunction asVertexStepFunc(VertexStepFunction::Code func);
};

} // namespace _priv
} // namespace Oryol