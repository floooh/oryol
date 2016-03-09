#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Pipeline
    @ingroup _priv
    @brief D3D11 implementation of pipeline
*/
#include "Gfx/Resource/pipelineBase.h"
#include "Gfx/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class d3d11Pipeline : public pipelineBase {
public:
    /// constructor
    d3d11Pipeline();
    /// destructor
    ~d3d11Pipeline();

    /// clear the object (called from pipelineFactory::DestroyResource())
    void Clear();

    /// pointer to input layout object
    ID3D11InputLayout* d3d11InputLayout;
    /// primitive topology
    D3D_PRIMITIVE_TOPOLOGY d3d11PrimTopology;
    /// rasterize state object
    ID3D11RasterizerState* d3d11RasterizerState;
    /// depth-stencil state object
    ID3D11DepthStencilState* d3d11DepthStencilState;
    /// blend state object
    ID3D11BlendState* d3d11BlendState;
};

} // namespace _priv
} // namespace Oryol
