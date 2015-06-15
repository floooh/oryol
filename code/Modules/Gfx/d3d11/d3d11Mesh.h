#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Mesh
    @ingroup _priv
    @brief D3D11 implementation of mesh
*/
#include "Gfx/Resource/meshBase.h"
#include "Gfx/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class d3d11Mesh : public meshBase {
public:
    /// constructor
    d3d11Mesh();
    /// destructor
    ~d3d11Mesh();

    /// clear the object (called from meshFactory::DestroyResource())
    void Clear();

    /// pointer to vertex buffer object
    ID3D11Buffer* d3d11VertexBuffer;
    /// pointer to optional index buffer object (nullptr if no indices)
    ID3D11Buffer* d3d11IndexBuffer;
};

} // namespace _priv
} // namespace Oryol
