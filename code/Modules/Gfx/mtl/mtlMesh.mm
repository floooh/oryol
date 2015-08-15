//------------------------------------------------------------------------------
//  mtlMesh.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlMesh.h"
#include "mtl_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlMesh::buffer::buffer() :
updateFrameIndex(-1),
numSlots(1),
activeSlot(0) {

    this->mtlBuffers.Fill(nil);
}

//------------------------------------------------------------------------------
mtlMesh::~mtlMesh() {
    #if ORYOL_DEBUG
    for (const auto& buf : this->buffers) {
        for (int i = 0; i < NumSlots; i++) {
            o_assert_dbg(buf.mtlBuffers[i] == nil);
        }
    }
    #endif
}

//------------------------------------------------------------------------------
void
mtlMesh::Clear() {
    for (auto& buf : this->buffers) {
        buf = buffer();
    }
    meshBase::Clear();
}

} // namespace _priv
} // namespace Oryol