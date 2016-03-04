//------------------------------------------------------------------------------
//  glMesh.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glMesh.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glMesh::~glMesh() {
    #if ORYOL_DEBUG
    for (const auto& buf : this->buffers) {
        for (int i = 0; i < MaxNumSlots; i++) {
            o_assert_dbg(0 == buf.glBuffers[i]);
        }
    }
    #endif
}

//------------------------------------------------------------------------------
void
glMesh::Clear() {
    for (auto& buf : this->buffers) {
        buf = buffer();
    }
    meshBase::Clear();
}

} // namespace _priv
} // namespace Oryol
