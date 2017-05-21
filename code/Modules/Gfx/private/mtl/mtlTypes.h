#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTypes
    @ingroup _priv
    @brief Oryol to Metal type mapping
    
    NOTE: only include this file from a Objective-C(++) file!
*/
#include "Gfx/GfxTypes.h"
#include "Gfx/private/mtl/mtl_decl.h"

#if !defined(__OBJC__)
#error "mtlTypes.h: Must be included from Obj-C source!"
#endif

namespace Oryol {
namespace _priv {

class mtlTypes {
public:
    /// convert color render target pixel format
    static MTLPixelFormat asRenderTargetColorFormat(PixelFormat::Code fmt);
    /// convert depth render target pixel format
    static MTLPixelFormat asRenderTargetDepthFormat(PixelFormat::Code fmt);
    /// convert stencil render target pixel format
    static MTLPixelFormat asRenderTargetStencilFormat(PixelFormat::Code fmt);
    /// convert texture pixel format
    static MTLPixelFormat asTextureFormat(PixelFormat::Code fmt);
    /// convert texture type
    static MTLTextureType asTextureType(TextureType::Code type);
    /// convert texture address mode
    static MTLSamplerAddressMode asSamplerAddressMode(TextureWrapMode::Code mode);
    /// convert to sampler min/mag filter
    static MTLSamplerMinMagFilter asSamplerMinMagFilter(TextureFilterMode::Code f);
    /// convert to sampler mip filter
    static MTLSamplerMipFilter asSamplerMipFilter(TextureFilterMode::Code f);
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
    /// convert cullmode
    static MTLCullMode asCullMode(bool cullEnabled, Face::Code face);
    /// convert vertex index type
    static MTLIndexType asIndexType(IndexType::Code c);
    /// convert primitive type
    static MTLPrimitiveType asPrimitiveType(PrimitiveType::Code c);
    /// convert render pass load action from PassAction flags
    static MTLLoadAction asLoadAction(const PassAction* action, int colorIndex, bool depthStencil);
};

} // namespace _priv
} // namespace Oryol
