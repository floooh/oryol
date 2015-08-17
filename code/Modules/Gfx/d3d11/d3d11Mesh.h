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

    ID3D11Buffer* d3d11VertexBuffer;
    ID3D11Buffer* d3d11IndexBuffer;
    int32 vbUpdateFrameIndex;
    int32 ibUpdateFrameIndex;
};

} // namespace _priv
} // namespace Oryol
