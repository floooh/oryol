#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Types
    @ingroup _priv
    @brief D3D11 enum conversion helpers
*/
#include "Core/Types.h"
#include "Gfx/GfxTypes.h"
#include "Gfx/private/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class d3d11Types {
public:
    /// convert PixelFormat to a display DXGI format
    static DXGI_FORMAT asSwapChainFormat(PixelFormat::Code pixelFormat);
    /// convert PixelFormat to a rendertarget DXGI format
    static DXGI_FORMAT asRenderTargetFormat(PixelFormat::Code pixelFormat);
    /// convert PixelFormat to a D3D11 texture format
    static DXGI_FORMAT asTextureFormat(PixelFormat::Code pixelFormat);
    /// convert Usage to a d3d11 usage for buffers and textures
    static D3D11_USAGE asResourceUsage(Usage::Code usage);
    /// convert Usage to a d3d11 CPU access flags mask
    static uint32_t asResourceCPUAccessFlag(Usage::Code usage);
    /// convert vertex attribute to d3d11 semantic name
    static const char* asSemanticName(VertexAttr::Code attr);
    /// convert vertex attribute to d3d11 semantic index
    static uint32_t asSemanticIndex(VertexAttr::Code attr);
    /// convert vertex component format to d3d11 input element DXGI_FORMAT
    static DXGI_FORMAT asInputElementFormat(VertexFormat::Code fmt);
    /// convert polygon face to d3d11 cull mode
    static D3D11_CULL_MODE asCullMode(bool cullEnabled, Face::Code face);
    /// convert CompareFunc to d3d11 comparison func
    static D3D11_COMPARISON_FUNC asComparisonFunc(CompareFunc::Code func);
    /// convert stencil-op to d3d11 stencil-op
    static D3D11_STENCIL_OP asStencilOp(StencilOp::Code op);
    /// convert blend factor to d3d11 blend factor
    static D3D11_BLEND asBlendFactor(BlendFactor::Code b);
    /// convert blend operation to d3d11 blend op
    static D3D11_BLEND_OP asBlendOp(BlendOperation::Code op);
    /// convert PixelChannel::Mask to d3d11 color write mask
    static uint8_t asColorWriteMask(PixelChannel::Mask mask);
    /// convert VertexStepFunction to d3d11 input classification
    static D3D11_INPUT_CLASSIFICATION asInputClassification(VertexStepFunction::Code func);
    /// convert min/mag filters to d3d11 filter
    static D3D11_FILTER asSamplerFilter(TextureFilterMode::Code magFilter, TextureFilterMode::Code minFilter);
    /// convert texture wrap mode to d3d11 texture address mode
    static D3D11_TEXTURE_ADDRESS_MODE asTextureAddressMode(TextureWrapMode::Code mode);
    /// convert vertex index type to dxgi format
    static DXGI_FORMAT asIndexType(IndexType::Code c);
    /// convert primitive type
    static D3D11_PRIMITIVE_TOPOLOGY asPrimitiveTopology(PrimitiveType::Code c);
};

} // namespace _priv 
} // namespace Oryol