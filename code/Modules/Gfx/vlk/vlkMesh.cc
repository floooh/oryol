//------------------------------------------------------------------------------
//  vlkMesh.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlkMesh.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkMesh::~vlkMesh() {
    #if ORYOL_DEBUG
    for (const auto& buf : this->buffers) {
        for (const auto& slot : buf.slots) {
            o_assert(!slot.renderBuffer);
            o_assert(!slot.stagingBuffer);
            o_assert(!slot.renderBufferMemory);
            o_assert(!slot.stagingBufferMemory);
        }
    }
    #endif
}

//------------------------------------------------------------------------------
void
vlkMesh::Clear() {
    for (auto& buf : this->buffers) {
        buf = buffer();
    }
    meshBase::Clear();
}

} // namespace _priv
} // namespace Oryol
