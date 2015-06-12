//------------------------------------------------------------------------------
//  glMesh.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glMesh.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
glMesh::glMesh() :
glIndexBuffer(0),
numVertexBufferSlots(1),
activeVertexBufferSlot(0) {
    this->glVertexBuffers.Fill(0);
}

//------------------------------------------------------------------------------
glMesh::~glMesh() {
    o_assert_dbg(0 == this->glIndexBuffer);
    for (uint32 i = 0; i < MaxNumSlots; i++) {
        o_assert_dbg(0 == this->glVertexBuffers[i]);
    }
}

//------------------------------------------------------------------------------
void
glMesh::Clear() {
    this->numVertexBufferSlots = 1;
    this->activeVertexBufferSlot = 0;
    this->glVertexBuffers.Fill(0);
    this->glIndexBuffer = 0;
    meshBase::Clear();
}

} // namespace _priv
} // namespace Oryol
