#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11DrawState
    @ingroup _priv
    @brief D3D11 implementation of drawState
*/
#include "Gfx/Resource/drawStateBase.h"
#include "Gfx/d3d11/d3d11_decl.h"
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

    /// pointer to input layout object
    ID3D11InputLayout* d3d11InputLayout;
    /// pointer to vertex shader
    ID3D11VertexShader* d3d11VertexShader;
    /// pointer to pixel shader
    ID3D11PixelShader* d3d11PixelShader;
    /// rasterize state object
    ID3D11RasterizerState* d3d11RasterizerState;
    /// depth-stencil state object
    ID3D11DepthStencilState* d3d11DepthStencilState;
    /// blend state object
    ID3D11BlendState* d3d11BlendState;
    /// input assembler vertex buffer pointers
    StaticArray<ID3D11Buffer*, GfxConfig::MaxNumInputMeshes> d3d11IAVertexBuffers;
    /// input assember vertex buffer strides
    StaticArray<uint32, GfxConfig::MaxNumInputMeshes> d3d11IAStrides;
    /// input assembler vertex buffer offsets
    StaticArray<uint32, GfxConfig::MaxNumInputMeshes> d3d11IAOffsets;
};

} // namespace _priv
} // namespace Oryol
