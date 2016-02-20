//------------------------------------------------------------------------------
//  d3d12Mesh.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12Mesh.h"
#include "d3d12_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12Mesh::d3d12Mesh() :
d3d12PrimTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED),
d3d12PrimTopology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED) {
    // empty
}

//------------------------------------------------------------------------------
d3d12Mesh::~d3d12Mesh() {
    #if ORYOL_DEBUG
    for (const auto& buf : this->buffers) {
        for (int i = 0; i < NumSlots; i++) {
            o_assert_dbg(nullptr == buf.d3d12RenderBuffers[i]);
            o_assert_dbg(nullptr == buf.d3d12UploadBuffers[i]);
        }
    }
    #endif
}

//------------------------------------------------------------------------------
void
d3d12Mesh::Clear() {
    for (auto& buf : this->buffers) {
        buf = buffer();
    }
    this->d3d12PrimTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    this->d3d12PrimTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    meshBase::Clear();
}

} // namespace _priv
} // namespace Oryol
