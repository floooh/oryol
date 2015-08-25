#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12Types
    @ingroup _priv
    @brief D3D12 enum conversion helpers
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/d3d12/d3d12_decl.h"

namespace Oryol {
namespace _priv {

class d3d12Types {
public:
    /// convert PixelFormat to a display DXGI format
    static DXGI_FORMAT asSwapChainFormat(PixelFormat::Code pixelFormat);
    /// convert PixelFormat to a rendertarget DXGI format
    static DXGI_FORMAT asRenderTargetFormat(PixelFormat::Code pixelFormat);
    /// convert blend factor to d3d12 blend factor
    static D3D12_BLEND asBlendFactor(BlendFactor::Code b);
    /// convert blend operation to d3d12 blend op
    static D3D12_BLEND_OP asBlendOp(BlendOperation::Code op);
    /// convert PixelChannel::Mask to d3d12 color write mask
    static uint8 asColorWriteMask(PixelChannel::Mask mask);
    /// convert polygon face to d3d11 cull mode
    static D3D12_CULL_MODE asCullMode(bool cullEnabled, Face::Code face);
    /// convert CompareFunc to d3d11 comparison func
    static D3D12_COMPARISON_FUNC asComparisonFunc(CompareFunc::Code func);
    /// convert stencil-op to d3d11 stencil-op
    static D3D12_STENCIL_OP asStencilOp(StencilOp::Code op);
    /// convert primitive topology
    static D3D12_PRIMITIVE_TOPOLOGY asPrimitiveTopology(PrimitiveType::Code c);
    /// convert primitive topology TYPE (this is not the same as primitive topology!)
    static D3D12_PRIMITIVE_TOPOLOGY_TYPE asPrimitiveTopologyType(PrimitiveType::Code c);
};

} // namespace _priv
} // namespace Oryol