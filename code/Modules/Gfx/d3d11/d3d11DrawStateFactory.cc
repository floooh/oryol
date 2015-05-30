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
        HRESULT hr = this->d3d11Device->CreateInputLayout(
            d3d11Comps,         // pInputElementDesc 
            d3d11CompIndex,     // NumElements
            vsByteCode,         // pShaderBytecodeWithInputSignature 
            vsSize,             // BytecodeLength
            &ds.d3d11InputLayouts[progIndex]);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(nullptr != ds.d3d11InputLayouts[progIndex]);
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d11DrawStateFactory::DestroyResource(drawState& ds) {
    o_assert_dbg(nullptr != this->renderer);

    this->renderer->invalidateInputLayoutState();
    for (int i = 0; i < programBundle::MaxNumPrograms; i++) {
        if (nullptr != ds.d3d11InputLayouts[i]) {
            ds.d3d11InputLayouts[i]->Release();
        }
    }

    drawStateFactoryBase::DestroyResource(ds);
}

} // namespace _priv
} // namespace Oryol