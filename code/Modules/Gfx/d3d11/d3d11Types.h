#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Types
    @ingroup _priv
    @brief D3D11 enum conversion helpers
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class d3d11Types {
public:
    /// convert PixelFormat to a display DXGI format
    static DXGI_FORMAT asSwapChainFormat(PixelFormat::Code pixelFormat);
    /// convert PixelFormat to a D3D11 texture format
    static DXGI_FORMAT asTextureFormat(PixelFormat::Code pixelFormat);
    /// convert Usage to a d3d11 usage for buffers
    static D3D11_USAGE asBufferUsage(Usage::Code usage);
    /// convert Usage to a d3d11 buffer CPU access flags mask
    static uint32 asBufferCPUAccessFlag(Usage::Code usage);
    /// convert vertex attribute to d3d11 semantic name
    static const char* asSemanticName(VertexAttr::Code attr);
    /// convert vertex attribute to d3d11 semantic index
    static uint32 asSemanticIndex(VertexAttr::Code attr);
    /// convert vertex component format to d3d11 input element DXGI_FORMAT
    static DXGI_FORMAT asInputElementFormat(VertexFormat::Code fmt);
    /// convert polygon face to d3d11 cull mode
    static D3D11_CULL_MODE asCullMode(Face::Code face);
    /// convert CompareFunc to d3d11 comparison func
    static D3D11_COMPARISON_FUNC asComparisonFunc(CompareFunc::Code func);
    /// convert stencil-op to d3d11 stencil-op
    static D3D11_STENCIL_OP asStencilOp(StencilOp::Code op);
    /// convert blend factor to d3d11 blend factor
    static D3D11_BLEND asBlendFactor(BlendFactor::Code b);
    /// convert blend operation to d3d11 blend op
    static D3D11_BLEND_OP asBlendOp(BlendOperation::Code op);
    /// convert PixelChannel::Mask to d3d11 color write mask
    static uint8 asColorWriteMask(PixelChannel::Mask mask);
};

} // namespace _priv 
} // namespace Oryol