//------------------------------------------------------------------------------
//  d3d11DrawStateFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11DrawStateFactory.h"
#include "d3d11_impl.h"
#include "d3d11Types.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/drawState.h"
#include "Gfx/Resource/programBundle.h"
#include "Gfx/Resource/meshPool.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11DrawStateFactory::d3d11DrawStateFactory() :
d3d11Device(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
d3d11DrawStateFactory::~d3d11DrawStateFactory() {
    o_assert_dbg(nullptr == this->d3d11Device);
}

//------------------------------------------------------------------------------
void
d3d11DrawStateFactory::Setup(class renderer* rendr, class meshPool* mshPool, class programBundlePool* pbPool) {
    o_assert_dbg(nullptr == this->d3d11Device);

    drawStateFactoryBase::Setup(rendr, mshPool, pbPool);
    this->d3d11Device = rendr->d3d11Device;
    o_assert_dbg(nullptr != this->d3d11Device);
}

//------------------------------------------------------------------------------
void
d3d11DrawStateFactory::Discard() {
    o_assert_dbg(nullptr != this->d3d11Device);
    this->d3d11Device = nullptr;
    drawStateFactoryBase::Discard();
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11DrawStateFactory::SetupResource(drawState& ds) {
    o_assert_dbg(nullptr != this->d3d11Device);
    o_assert_dbg(nullptr == ds.d3d11RasterizerState);
    o_assert_dbg(nullptr == ds.d3d11BlendState);
    o_assert_dbg(nullptr == ds.d3d11DepthStencilState);
    HRESULT hr;

    drawStateFactoryBase::SetupResource(ds);
    const mesh* msh = this->meshPool->Lookup(ds.msh);
    o_assert(msh);
    const VertexLayout& mshLayout = msh->Setup.Layout;

    // create input layout objects
    // FIXME: this should be rewritten to enable shared input layouts 
    // per shader signature/mesh signature pairs, which would reduce
    // the number of input layouts dramatically
    o_assert_dbg(ds.prog);
    const int numProgEntries = ds.prog->getNumPrograms();
    for (int progIndex = 0; progIndex < numProgEntries; progIndex++) {
        o_assert_dbg(nullptr == ds.d3d11InputLayouts[progIndex]);

        const void* vsByteCode = nullptr;
        uint32 vsSize = 0;
        ds.prog->Setup.VertexShaderByteCode(progIndex, ShaderLang::HLSL5, vsByteCode, vsSize);
        o_assert_dbg(vsByteCode && (vsSize > 0));

        // build the vertex layout for mesh and optional instance data mesh
        D3D11_INPUT_ELEMENT_DESC d3d11Comps[VertexLayout::MaxNumVertexComponents * 2];
        Memory::Clear(d3d11Comps, sizeof(d3d11Comps));
        int d3d11CompIndex = 0;
        for (int compIndex = 0; compIndex < mshLayout.NumComponents(); compIndex++, d3d11CompIndex++) {
            const VertexComponent& comp = mshLayout.Component(compIndex);
            D3D11_INPUT_ELEMENT_DESC& d3d11Comp = d3d11Comps[d3d11CompIndex];
            d3d11Comp.SemanticName = d3d11Types::asSemanticName(comp.Attr);
            d3d11Comp.SemanticIndex = d3d11Types::asSemanticIndex(comp.Attr);
            d3d11Comp.Format = d3d11Types::asInputElementFormat(comp.Format);
            d3d11Comp.InputSlot = 0;
            d3d11Comp.AlignedByteOffset = mshLayout.ComponentByteOffset(compIndex);
            d3d11Comp.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            d3d11Comp.InstanceDataStepRate = 0;
        }
        // append the vertex layout for the optional instance data
        if (msh->instanceMesh) {
            const VertexLayout& instLayout = msh->instanceMesh->Setup.Layout;
            for (int compIndex = 0; compIndex < instLayout.NumComponents(); compIndex++, d3d11CompIndex++) {
                const VertexComponent& comp = instLayout.Component(compIndex);
                D3D11_INPUT_ELEMENT_DESC& d3d11Comp = d3d11Comps[d3d11CompIndex];
                d3d11Comp.SemanticName = d3d11Types::asSemanticName(comp.Attr);
                d3d11Comp.SemanticIndex = d3d11Types::asSemanticIndex(comp.Attr);
                d3d11Comp.Format = d3d11Types::asInputElementFormat(comp.Format);
                d3d11Comp.InputSlot = 1;
                d3d11Comp.AlignedByteOffset = instLayout.ComponentByteOffset(compIndex);
                d3d11Comp.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
                d3d11Comp.InstanceDataStepRate = 1;
            }
        }

        // create d3d11 input layout object
        hr = this->d3d11Device->CreateInputLayout(
            d3d11Comps,         // pInputElementDesc 
            d3d11CompIndex,     // NumElements
            vsByteCode,         // pShaderBytecodeWithInputSignature 
            vsSize,             // BytecodeLength
            &ds.d3d11InputLayouts[progIndex]);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(nullptr != ds.d3d11InputLayouts[progIndex]);
    }

    // create state objects (NOTE: creating the same state will return
    // the same d3d11 state object, so no need to implement our own reuse)
    const RasterizerState& rastState = ds.Setup.RasterizerState;
    D3D11_RASTERIZER_DESC rastDesc;
    Memory::Clear(&rastDesc, sizeof(rastDesc));
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = d3d11Types::asCullMode(rastState.CullFace);
    rastDesc.FrontCounterClockwise = TRUE;  // ???
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.ScissorEnable = rastState.ScissorTestEnabled;
    rastDesc.MultisampleEnable = rastState.MultisampleEnabled;
    rastDesc.AntialiasedLineEnable = FALSE;
    hr = this->d3d11Device->CreateRasterizerState(&rastDesc, &ds.d3d11RasterizerState);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != ds.d3d11RasterizerState);

    const DepthStencilState& dsState = ds.Setup.DepthStencilState;
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    Memory::Clear(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = TRUE;      // FIXME: have DepthTestEnable in RasterizerState?
    dsDesc.DepthWriteMask = dsState.DepthWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = d3d11Types::asComparisonFunc(dsState.DepthCmpFunc);
    // FIXME: move StencilEnabled, StencilReadMask, StencilWriteMask up, not per side
    dsDesc.StencilEnable    = dsState.StencilFront.StencilEnabled;
    dsDesc.StencilReadMask  = dsState.StencilFront.ReadMask;
    dsDesc.StencilWriteMask = dsState.StencilFront.WriteMask;
    dsDesc.FrontFace.StencilFailOp = d3d11Types::asStencilOp(dsState.StencilFront.FailOp);
    dsDesc.FrontFace.StencilDepthFailOp = d3d11Types::asStencilOp(dsState.StencilFront.DepthFailOp);
    dsDesc.FrontFace.StencilPassOp = d3d11Types::asStencilOp(dsState.StencilFront.PassOp);
    dsDesc.FrontFace.StencilFunc = d3d11Types::asComparisonFunc(dsState.StencilFront.CmpFunc);
    dsDesc.BackFace.StencilFailOp = d3d11Types::asStencilOp(dsState.StencilBack.FailOp);
    dsDesc.BackFace.StencilDepthFailOp = d3d11Types::asStencilOp(dsState.StencilBack.DepthFailOp);
    dsDesc.BackFace.StencilPassOp = d3d11Types::asStencilOp(dsState.StencilBack.PassOp);
    dsDesc.BackFace.StencilFunc = d3d11Types::asComparisonFunc(dsState.StencilBack.CmpFunc);
    hr = this->d3d11Device->CreateDepthStencilState(&dsDesc, &ds.d3d11DepthStencilState);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != ds.d3d11DepthStencilState);

    const BlendState& blendState = ds.Setup.BlendState;
    D3D11_BLEND_DESC blendDesc;
    Memory::Clear(&blendDesc, sizeof(blendDesc));
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    blendDesc.RenderTarget[0].BlendEnable = blendState.BlendEnabled;
    blendDesc.RenderTarget[0].SrcBlend = d3d11Types::asBlendFactor(blendState.SrcFactorRGB);
    blendDesc.RenderTarget[0].DestBlend = d3d11Types::asBlendFactor(blendState.DstFactorRGB);
    blendDesc.RenderTarget[0].BlendOp = d3d11Types::asBlendOp(blendState.OpRGB);
    blendDesc.RenderTarget[0].SrcBlendAlpha = d3d11Types::asBlendFactor(blendState.SrcFactorAlpha);
    blendDesc.RenderTarget[0].DestBlendAlpha = d3d11Types::asBlendFactor(blendState.DstFactorAlpha);
    blendDesc.RenderTarget[0].BlendOpAlpha = d3d11Types::asBlendOp(blendState.OpAlpha);
    blendDesc.RenderTarget[0].RenderTargetWriteMask = d3d11Types::asColorWriteMask(blendState.ColorWriteMask);
    hr = this->d3d11Device->CreateBlendState(&blendDesc, &ds.d3d11BlendState);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != ds.d3d11BlendState);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d11DrawStateFactory::DestroyResource(drawState& ds) {
    o_assert_dbg(nullptr != this->renderer);

    this->renderer->invalidateDrawState();
    for (int i = 0; i < programBundle::MaxNumPrograms; i++) {
        if (nullptr != ds.d3d11InputLayouts[i]) {
            ds.d3d11InputLayouts[i]->Release();
        }
    }
    if (nullptr != ds.d3d11RasterizerState) {
        ds.d3d11RasterizerState->Release();
    }
    if (nullptr != ds.d3d11BlendState) {
        ds.d3d11BlendState->Release();
    }
    if (nullptr != ds.d3d11DepthStencilState) {
        ds.d3d11DepthStencilState->Release();
    }
    drawStateFactoryBase::DestroyResource(ds);
}

} // namespace _priv
} // namespace Oryol