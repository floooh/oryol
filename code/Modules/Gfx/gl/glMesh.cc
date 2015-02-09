//------------------------------------------------------------------------------
//  glMesh.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glMesh.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
glMesh::glMesh() :
numVertexBufferSlots(1),
activeVertexBufferSlot(0),
numVAOSlots(1),
glIndexBuffer(0),
instanceMesh(nullptr),
activeVAOSlot(0) {
    this->glVertexBuffers.Fill(0);
    this->glVAOs.Fill(0);
}

//------------------------------------------------------------------------------
glMesh::~glMesh() {
    o_assert_dbg(0 == this->glIndexBuffer);
    o_assert_dbg(nullptr == this->instanceMesh);
    for (uint32 i = 0; i < MaxNumSlots; i++) {
        o_assert_dbg(0 == this->glVertexBuffer[i]);
        o_assert_dbg(0 == this->glVAO[i]);
    }
}

//------------------------------------------------------------------------------
void
glMesh::clear() {
    this->numVertexBufferSlots = 1;
    this->activeVertexBufferSlot = 0;
    this->numVAOSlots = 1;
    this->activeVAOSlot = 0;
    this->glVertexBuffers.Fill(0);
    this->glVAOs.Fill(0);
    this->glIndexBuffer = 0;
    this->instanceMesh = nullptr;
    for (int32 slotIndex = 0; slotIndex < MaxNumSlots; slotIndex++) {
        for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
            this->glAttrs[slotIndex][i] = glVertexAttr();
        }
    }
    meshBase::clear();
}

} // namespace _priv
} // namespace Oryol
