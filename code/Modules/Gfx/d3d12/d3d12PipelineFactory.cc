//------------------------------------------------------------------------------
//  d3d12PipelineFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "Gfx/Resource/pipeline.h"
#include "Gfx/Resource/shader.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/gfxResourceContainer.h"
#include "d3d12PipelineFactory.h"
#include "d3d12ResAllocator.h"
#include "d3d12Types.h"
#include "d3d12_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
static D3D12_DEPTH_STENCILOP_DESC
asStencilOpDesc(const StencilState& stencilState) {
    D3D12_DEPTH_STENCILOP_DESC out;
    Memory::Clear(&out, sizeof(out));
    out.StencilFailOp = d3d12Types::asStencilOp(stencilState.FailOp);
    out.StencilDepthFailOp = d3d12Types::asStencilOp(stencilState.DepthFailOp);
    out.StencilPassOp = d3d12Types::asStencilOp(stencilState.PassOp);
    out.StencilFunc = d3d12Types::asComparisonFunc(stencilState.CmpFunc);
    return out;
}

//------------------------------------------------------------------------------
static UINT
describeInputLayout(pipeline& pip, D3D12_INPUT_ELEMENT_DESC* inputLayout) {

    int d3d12CompIndex = 0;
    for (int mshSlotIndex = 0; mshSlotIndex < GfxConfig::MaxNumInputMeshes; mshSlotIndex++) {
        const VertexLayout& layout = pip.Setup.Layouts[mshSlotIndex];
        for (int compIndex = 0; compIndex < layout.NumComponents(); compIndex++, d3d12CompIndex++) {
            const auto& comp = layout.ComponentAt(compIndex);
            o_assert_dbg(d3d12CompIndex < VertexAttr::NumVertexAttrs);
            D3D12_INPUT_ELEMENT_DESC& d3d12Comp = inputLayout[d3d12CompIndex];
            d3d12Comp.SemanticName = d3d12Types::asSemanticName(comp.Attr);
            d3d12Comp.SemanticIndex = d3d12Types::asSemanticIndex(comp.Attr);
            d3d12Comp.Format = d3d12Types::asInputElementFormat(comp.Format);
            d3d12Comp.InputSlot = mshSlotIndex;
            d3d12Comp.AlignedByteOffset = layout.ComponentByteOffset(compIndex);
            d3d12Comp.InputSlotClass = d3d12Types::asInputClassification(layout.StepFunction);
            if (VertexStepFunction::PerVertex == layout.StepFunction) {
                d3d12Comp.InstanceDataStepRate = 0;
            }
            else {
                d3d12Comp.InstanceDataStepRate = layout.StepRate;
            }
        }
    }
    return d3d12CompIndex;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12PipelineFactory::SetupResource(pipeline& pip) {
    pipelineFactoryBase::SetupResource(pip);
    o_assert_dbg(pip.shd);
    this->createPSO(pip);
    o_assert_dbg(pip.d3d12PipelineState);
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d12PipelineFactory::DestroyResource(pipeline& pip) {
    o_assert_dbg(this->isValid);
    if (pip.d3d12PipelineState) {
        d3d12ResAllocator& resAllocator = this->pointers.renderer->resAllocator;
        const uint64 frameIndex = this->pointers.renderer->frameIndex;
        resAllocator.ReleaseDeferred(frameIndex, pip.d3d12PipelineState);
    }
    pipelineFactoryBase::DestroyResource(pip);
}

//------------------------------------------------------------------------------
void
d3d12PipelineFactory::createPSO(pipeline& pip) {
    o_assert_dbg(nullptr == pip.d3d12PipelineState);
    o_assert_dbg(this->pointers.renderer->d3d12RootSignature);

    // create new PSO
    ID3D12Device* d3d12Device = this->pointers.renderer->d3d12Device;
    o_assert_dbg(d3d12Device);

    // setup input-layout-desc
    D3D12_INPUT_ELEMENT_DESC inputLayout[VertexAttr::NumVertexAttrs];
    Memory::Clear(inputLayout, sizeof(inputLayout));
    UINT inputLayoutNumElements = describeInputLayout(pip, inputLayout);
    pip.d3d12PrimTopologyType = d3d12Types::asPrimitiveTopologyType(pip.Setup.PrimType);
    pip.d3d12PrimTopology = d3d12Types::asPrimitiveTopology(pip.Setup.PrimType);

    // get vertex and pixel shader byte code
    const shader::shaderBlob& vs = pip.shd->vertexShader;
    const shader::shaderBlob& ps = pip.shd->pixelShader;
    o_assert_dbg(vs.ptr && (vs.size > 0) && ps.ptr && (ps.size > 0));

    // create the pipeline-state-object
    const RasterizerState& rs = pip.Setup.RasterizerState;
    const BlendState& bs = pip.Setup.BlendState;
    const DepthStencilState& dss = pip.Setup.DepthStencilState;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    Memory::Clear(&psoDesc, sizeof(psoDesc));
    psoDesc.pRootSignature = this->pointers.renderer->d3d12RootSignature;
    psoDesc.VS.pShaderBytecode = vs.ptr;
    psoDesc.VS.BytecodeLength = vs.size;
    psoDesc.PS.pShaderBytecode = ps.ptr;
    psoDesc.PS.BytecodeLength = ps.size;
    psoDesc.BlendState.AlphaToCoverageEnable = rs.AlphaToCoverageEnabled;
    psoDesc.BlendState.IndependentBlendEnable = FALSE;
    psoDesc.BlendState.RenderTarget[0].BlendEnable = bs.BlendEnabled;
    psoDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
    psoDesc.BlendState.RenderTarget[0].SrcBlend = d3d12Types::asBlendFactor(bs.SrcFactorRGB);
    psoDesc.BlendState.RenderTarget[0].DestBlend = d3d12Types::asBlendFactor(bs.DstFactorRGB);
    psoDesc.BlendState.RenderTarget[0].BlendOp = d3d12Types::asBlendOp(bs.OpRGB);
    psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = d3d12Types::asBlendFactor(bs.SrcFactorAlpha);
    psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = d3d12Types::asBlendFactor(bs.DstFactorAlpha);
    psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = d3d12Types::asBlendOp(bs.OpAlpha);
    psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = d3d12Types::asColorWriteMask(bs.ColorWriteMask);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    psoDesc.RasterizerState.CullMode = d3d12Types::asCullMode(rs.CullFaceEnabled, rs.CullFace);
    psoDesc.RasterizerState.FrontCounterClockwise = FALSE;  // OpenGL convention
    psoDesc.RasterizerState.DepthBias = 0;
    psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
    psoDesc.RasterizerState.SlopeScaledDepthBias = 0.0f;
    psoDesc.RasterizerState.DepthClipEnable = TRUE;
    psoDesc.RasterizerState.MultisampleEnable = rs.SampleCount > 1;
    psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
    psoDesc.RasterizerState.ForcedSampleCount = 0;
    psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    psoDesc.DepthStencilState.DepthEnable = (dss.DepthCmpFunc != CompareFunc::Never);
    psoDesc.DepthStencilState.DepthWriteMask = dss.DepthWriteEnabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    psoDesc.DepthStencilState.DepthFunc = d3d12Types::asComparisonFunc(dss.DepthCmpFunc);
    psoDesc.DepthStencilState.StencilEnable = dss.StencilEnabled;
    psoDesc.DepthStencilState.StencilReadMask = dss.StencilReadMask;
    psoDesc.DepthStencilState.StencilWriteMask = dss.StencilWriteMask;
    psoDesc.DepthStencilState.FrontFace = asStencilOpDesc(dss.StencilFront);
    psoDesc.DepthStencilState.BackFace = asStencilOpDesc(dss.StencilBack);
    psoDesc.InputLayout.pInputElementDescs = inputLayout;
    psoDesc.InputLayout.NumElements = inputLayoutNumElements;
    psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    psoDesc.PrimitiveTopologyType = pip.d3d12PrimTopologyType;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = d3d12Types::asRenderTargetFormat(bs.ColorFormat);
    if (bs.DepthFormat != PixelFormat::InvalidPixelFormat) {
        psoDesc.DSVFormat = d3d12Types::asRenderTargetFormat(bs.DepthFormat);
    }
    psoDesc.SampleDesc.Count = rs.SampleCount;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.NodeMask = 0;
    psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hr = d3d12Device->CreateGraphicsPipelineState(&psoDesc, __uuidof(ID3D12PipelineState), (void**)&pip.d3d12PipelineState);
    o_assert(SUCCEEDED(hr) && pip.d3d12PipelineState);
}

} // namespace _priv
} // namespace Oryol
