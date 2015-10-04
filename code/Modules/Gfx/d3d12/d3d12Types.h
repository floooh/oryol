#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12Types
    @ingroup _priv
    @brief D3D12 enum conversion helpers
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/SamplerState.h"
#include "Gfx/Setup/TextureSetup.h"
#include "Gfx/Attrs/TextureAttrs.h"
#include "Gfx/d3d12/d3d12_decl.h"

namespace Oryol {
namespace _priv {

class d3d12Types {
public:
    /// convert PixelFormat to a display DXGI format
    static DXGI_FORMAT asSwapChainFormat(PixelFormat::Code pixelFormat);
    /// convert PixelFormat to a rendertarget DXGI format
    static DXGI_FORMAT asRenderTargetFormat(PixelFormat::Code pixelFormat);
    /// convert PixelFormat to a texture DXGI format
    static DXGI_FORMAT asTextureFormat(PixelFormat::Code pixelFormat);
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
    /// convert min/mag filters to d3d11 filter
    static D3D12_FILTER asSamplerFilter(TextureFilterMode::Code magFilter, TextureFilterMode::Code minFilter);
    /// convert texture wrap mode to d3d11 texture address mode
    static D3D12_TEXTURE_ADDRESS_MODE asTextureAddressMode(TextureWrapMode::Code mode);
    /// convert vertex index type to dxgi format
    static DXGI_FORMAT asIndexType(IndexType::Code c);
    /// initialize a D3D12_HEAP_PROPERTIES struct
    static void initHeapProps(D3D12_HEAP_PROPERTIES* out, D3D12_HEAP_TYPE type);
    /// initialize a D3D12_RESOURCE_DESC for a vertex/index/constant buffer
    static void initBufferResourceDesc(D3D12_RESOURCE_DESC* out, int size);
    /// initialize a D3D12_RESOURCE_DESC for a render-target surface
    static void initRTResourceDesc(D3D12_RESOURCE_DESC* out, int width, int height, PixelFormat::Code fmt, int sampleCount);
    /// initialize a D3D12_RESOURCE_DESC for a 2D or Cube texture
    static void initTextureResourceDesc(D3D12_RESOURCE_DESC* out, const TextureSetup& setup);
    /// initialize a D3D12_CLEAR_VALUE
    static void initClearValue(D3D12_CLEAR_VALUE* out, PixelFormat::Code fmt, const ClearState& clearState);
    // initialize a D3D12_DESCRIPTOR_HEAP_DESC 
    static void initDescriptorHeapDesc(D3D12_DESCRIPTOR_HEAP_DESC* out, int num, D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible);
    /// initialize a D3D12_DEPTH_STENCIL_VIEW_DESC
    static void initDSVDesc(D3D12_DEPTH_STENCIL_VIEW_DESC* out, PixelFormat::Code fmt, bool isMSAA);
    /// initialize a D3D12_SAMPLER_DESC
    static void initSamplerDesc(D3D12_SAMPLER_DESC* out, const SamplerState& sampler);
    /// initialize a D3D12_SHADER_RESOURCE_VIEW_DESC
    static void initSRVDesc(D3D12_SHADER_RESOURCE_VIEW_DESC* out, const TextureAttrs& attrs);
    /// initialize a root-signature descriptor range
    static void initDescriptorRange(D3D12_DESCRIPTOR_RANGE* out, D3D12_DESCRIPTOR_RANGE_TYPE type, int num, int reg, int regSpace);
    /// initialize a root-parameter as constant-buffer-view
    static void initRootParamAsCBV(D3D12_ROOT_PARAMETER* out, D3D12_SHADER_VISIBILITY vis, int reg, int regSpace);
    /// intitialize a root-parameter as descriptor table
    static void initRootParamAsTable(D3D12_ROOT_PARAMETER* out, D3D12_SHADER_VISIBILITY vis, D3D12_DESCRIPTOR_RANGE* ranges, int numRanges);
    /// initialize root-signature desc structure
    static void initRootDesc(D3D12_ROOT_SIGNATURE_DESC* out, D3D12_ROOT_PARAMETER* params, int numParams);
};

} // namespace _priv
} // namespace Oryol