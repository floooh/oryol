#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11DrawState
    @ingroup _priv
    @brief D3D11 implementation of drawState
*/
#include "Gfx/Resource/drawStateBase.h"
#include "Gfx/d3d11/d3d11_decl.h"
#include "Gfx/d3d11/d3d11ProgramBundle.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {
namespace _priv {

class d3d11DrawState : public drawStateBase {
public:
    /// constructor
    d3d11DrawState();
    /// destructor
    ~d3d11DrawState();

    /// clear the object (called from drawStateFactory::DestroyResource())
    void Clear();

    /// pointer to input layout objects (same number and order as programBundle variations)
    StaticArray<ID3D11InputLayout*, ProgramBundleSetup::MaxNumPrograms> d3d11InputLayouts;
    /// rasterize state object
    ID3D11RasterizerState* d3d11RasterizerState;
    /// depth-stencil state object
    ID3D11DepthStencilState* d3d11DepthStencilState;
    /// blend state object
    ID3D11BlendState* d3d11BlendState;
    /// input assembler vertex buffer pointers
    StaticArray<ID3D11Buffer*, DrawStateSetup::MaxInputMeshes> d3d11IAVertexBuffers;
    /// input assember vertex buffer strides
    StaticArray<uint32, DrawStateSetup::MaxInputMeshes> d3d11IAStrides;
    /// input assembler vertex buffer offsets
    StaticArray<uint32, DrawStateSetup::MaxInputMeshes> d3d11IAOffsets;
};

} // namespace _priv
} // namespace Oryol
