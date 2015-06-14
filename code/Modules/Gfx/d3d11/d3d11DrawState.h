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
    ID3D11InputLayout* d3d11InputLayouts[ProgramBundleSetup::MaxNumPrograms];
    /// rasterize state object
    ID3D11RasterizerState* d3d11RasterizerState;
    /// depth-stencil state object
    ID3D11DepthStencilState* d3d11DepthStencilState;
    /// blend state object
    ID3D11BlendState* d3d11BlendState;

    /// number of input assembler stage vertex buffer bindings
    int32 d3d11IANumSlots;
    /// input assembler vertex buffer pointers
    ID3D11Buffer* d3d11IAVertexBuffers[DrawStateSetup::MaxInputMeshes];
    /// input assember vertex buffer strides
    uint32 d3d11IAStrides[DrawStateSetup::MaxInputMeshes];
    /// input assembler vertex buffer offsets
    uint32 d3d11IAOffsets[DrawStateSetup::MaxInputMeshes];
};

} // namespace _priv
} // namespace Oryol
