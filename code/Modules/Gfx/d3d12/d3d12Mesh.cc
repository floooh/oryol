//------------------------------------------------------------------------------
//  d3d12Mesh.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12Mesh.h"

namespace Oryol {
namespace _priv {

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
    meshBase::Clear();
}

} // namespace _priv
} // namespace Oryol
