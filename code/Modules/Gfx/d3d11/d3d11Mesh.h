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
    /// destructor
    ~d3d11Mesh();

    /// clear the object (called from meshFactory::DestroyResource())
    void Clear();

    ID3D11Buffer* d3d11VertexBuffer = nullptr;
    ID3D11Buffer* d3d11IndexBuffer = nullptr;
    int32 vbUpdateFrameIndex = -1;
    int32 ibUpdateFrameIndex = -1;
};

} // namespace _priv
} // namespace Oryol
