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
};

} // namespace _priv
} // namespace Oryol