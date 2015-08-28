//------------------------------------------------------------------------------
//  d3d12DrawStateFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "Gfx/Resource/drawState.h"
#include "Gfx/Resource/shader.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/gfxResourceContainer.h"
#include "d3d12DrawStateFactory.h"
#include "d3d12ResourceAllocator.h"
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
describeInputLayout(drawState& ds, D3D12_INPUT_ELEMENT_DESC* inputLayout) {

    int d3d12CompIndex = 0;
    int d3d12IASlotIndex = 0;
    for (int mshIndex = 0; mshIndex < GfxConfig::MaxNumInputMeshes; mshIndex++) {
        const mesh* msh = ds.meshes[mshIndex];
        if (msh) {
            const VertexLayout& layout = msh->vertexBufferAttrs.Layout;
            for (int compIndex = 0; compIndex < layout.NumComponents(); compIndex++, d3d12CompIndex++) {
                const auto& comp = layout.ComponentAt(compIndex);
                o_assert_dbg(d3d12CompIndex < VertexAttr::NumVertexAttrs);
                D3D12_INPUT_ELEMENT_DESC& d3d12Comp = inputLayout[d3d12CompIndex];
                d3d12Comp.SemanticName = d3d12Types::asSemanticName(comp.Attr);
                d3d12Comp.SemanticIndex = d3d12Types::asSemanticIndex(comp.Attr);
                d3d12Comp.Format = d3d12Types::asInputElementFormat(comp.Format);
                d3d12Comp.InputSlot = d3d12IASlotIndex;
                d3d12Comp.AlignedByteOffset = layout.ComponentByteOffset(compIndex);
                d3d12Comp.InputSlotClass = d3d12Types::asInputClassification(msh->vertexBufferAttrs.StepFunction);
                if (VertexStepFunction::PerVertex == msh->vertexBufferAttrs.StepFunction) {
                    d3d12Comp.InstanceDataStepRate = 0;
                }
                else {
                    d3d12Comp.InstanceDataStepRate = msh->vertexBufferAttrs.StepRate;
                }
            }
            d3d12IASlotIndex++;
        }
    }
    return d3d12CompIndex;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12DrawStateFactory::SetupResource(drawState& ds) {

    drawStateFactoryBase::SetupResource(ds);
    o_assert_dbg(ds.shd);
    this->createPSO(ds);
    o_assert_dbg(ds.d3d12PipelineState);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d12DrawStateFactory::DestroyResource(drawState& ds) {
    o_assert_dbg(this->isValid);

    d3d12ResourceAllocator& resAllocator = this->pointers.resContainer->resAllocator;
    this->pointers.renderer->invalidateMeshState();
    const uint64 frameIndex = this->pointers.renderer->frameIndex;
    if (ds.d3d12PipelineState) {
        resAllocator.ReleaseDeferred(frameIndex, ds.d3d12PipelineState);
    }
    drawStateFactoryBase::DestroyResource(ds);
}

//------------------------------------------------------------------------------
void
d3d12DrawStateFactory::createPSO(drawState& ds) {
    o_assert_dbg(nullptr == ds.d3d12PipelineState);
    o_assert_dbg(this->pointers.renderer->d3d12RootSignature);

    ID3D12Device* d3d12Device = this->pointers.renderer->d3d12Device;
    o_assert_dbg(d3d12Device);

    // shader byte code pointers and size
    const void* vsPtr = nullptr;
    const void* fsPtr = nullptr;
    uint32 vsSize = 0, fsSize = 0;
    ds.shd->Setup.VertexShaderByteCode(0, ShaderLang::HLSL5, vsPtr, vsSize);
    ds.shd->Setup.FragmentShaderByteCode(0, ShaderLang::HLSL5, fsPtr, fsSize);
    o_assert_dbg(vsPtr && (vsSize > 0));
    o_assert_dbg(fsPtr && (fsSize > 0));

    // setup input-layout-desc
    D3D12_INPUT_ELEMENT_DESC inputLayout[VertexAttr::NumVertexAttrs];
    Memory::Clear(inputLayout, sizeof(inputLayout));
    UINT inputLayoutNumElements = describeInputLayout(ds, inputLayout);

    // FIXME: draw-state-setup must have shader-variation-mask! 
    // always use shader at slot 0 for now
    const RasterizerState& rs = ds.Setup.RasterizerState;
    const BlendState& bs = ds.Setup.BlendState;
    const DepthStencilState& dss = ds.Setup.DepthStencilState;

    // FIXME: ideally we'd want a pipeline cache here to re-use identical pipeline-state
    // since draw-states may contain identical pipeline-state but different 
    // bound vertex-buffers or textures:
    // - compute a hash over the D3D12_GRAPHICS_PIPELINE_STATE_DESC (but beware
    //   of the pointer to the input layout!)
    // - keep a dictionary of already compiled PSO objects with their DESC hash as key
    // - reuse PSO's if matching (use AddRef...?)
    // - in debug mode, or with optional cmake option, also compile redundant
    //   PSOs, and cross-check their blobs to make sure the hash is collision-safe!
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    Memory::Clear(&psoDesc, sizeof(psoDesc));
    psoDesc.pRootSignature = this->pointers.renderer->d3d12RootSignature;
    psoDesc.VS.pShaderBytecode = vsPtr;
    psoDesc.VS.BytecodeLength = vsSize;
    psoDesc.PS.pShaderBytecode = fsPtr;
    psoDesc.PS.BytecodeLength = fsSize;
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
    psoDesc.SampleMask = UINT_MAX;  // FIXME: ???
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
    psoDesc.DepthStencilState.DepthEnable = TRUE;   // FIXME: should this be dss.DepthCmpFund != NEVER?
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
    psoDesc.PrimitiveTopologyType = ds.meshes[0]->d3d12PrimTopologyType;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = d3d12Types::asRenderTargetFormat(bs.ColorFormat);
    psoDesc.DSVFormat = d3d12Types::asRenderTargetFormat(bs.DepthFormat);
    psoDesc.SampleDesc.Count = rs.SampleCount;
    o_warn("d3d12DrawStateFactory::SetupResource() what about the SampleDesc.Quality???\n");
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.NodeMask = 0;
    psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hr = d3d12Device->CreateGraphicsPipelineState(&psoDesc, __uuidof(ID3D12PipelineState), (void**)&ds.d3d12PipelineState);
    o_assert(SUCCEEDED(hr) && ds.d3d12PipelineState);
}

} // namespace _priv
} // namespace Oryol