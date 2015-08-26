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
    /// convert polygon face to d3d12 cull mode
    static D3D12_CULL_MODE asCullMode(bool cullEnabled, Face::Code face);
    /// convert CompareFunc to d3d12 comparison func
    static D3D12_COMPARISON_FUNC asComparisonFunc(CompareFunc::Code func);
    /// convert stencil-op to d3d12 stencil-op
    static D3D12_STENCIL_OP asStencilOp(StencilOp::Code op);
    /// convert primitive topology
    static D3D12_PRIMITIVE_TOPOLOGY asPrimitiveTopology(PrimitiveType::Code c);
    /// convert primitive topology TYPE (this is not the same as primitive topology!)
    static D3D12_PRIMITIVE_TOPOLOGY_TYPE asPrimitiveTopologyType(PrimitiveType::Code c);
    /// convert vertex attribute to d3d12 semantic name
    static const char* asSemanticName(VertexAttr::Code attr);
    /// convert vertex attribute to d3d12 semantic index
    static uint32 asSemanticIndex(VertexAttr::Code attr);
    /// convert vertex component format to d3d12 input element DXGI_FORMAT
    static DXGI_FORMAT asInputElementFormat(VertexFormat::Code fmt);
    /// convert VertexStepFunction to d3d11 input classification
    static D3D12_INPUT_CLASSIFICATION asInputClassification(VertexStepFunction::Code func);
};

} // namespace _priv
} // namespace Oryol