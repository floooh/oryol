//------------------------------------------------------------------------------
//  glMesh.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glMesh.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
glMesh::glMesh() :
numVertexBufferSlots(1),
activeVertexBufferSlot(0),
numVAOSlots(1),
activeVAOSlot(0),
glIndexBuffer(0),
instanceMesh(nullptr) {
    for (uint32 i = 0; i < MaxNumSlots; i++) {
        this->glVertexBuffer[i] = 0;
        this->glVAO[i] = 0;
    }
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
    for (uint32 i = 0; i < MaxNumSlots; i++) {
        this->glVertexBuffer[i] = 0;
        this->glVAO[i] = 0;
    }
    this->glIndexBuffer = 0;
    this->instanceMesh = nullptr;
    for (int32 slotIndex = 0; slotIndex < MaxNumSlots; slotIndex++) {
        for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
            this->glAttrs[slotIndex][i] = glVertexAttr();
        }
    }
    meshBase::clear();
}

//------------------------------------------------------------------------------
void
glMesh::glSetVertexBuffer(uint8 index, GLuint vb) {
    o_assert_dbg((index < MaxNumSlots) && (0 == this->glVertexBuffer[index]));
    this->glVertexBuffer[index] = vb;
}

//------------------------------------------------------------------------------
void
glMesh::glSetVAO(uint8 index, GLuint vao) {
    o_assert_dbg((index < MaxNumSlots) && (0 == this->glVAO[index]));
    this->glVAO[index] = vao;
}

//------------------------------------------------------------------------------
void
glMesh::glSetIndexBuffer(GLuint ib) {
    o_assert_dbg(0 == this->glIndexBuffer);
    this->glIndexBuffer = ib;
}

//------------------------------------------------------------------------------
void
glMesh::glSetAttr(uint8 vaoSlotIndex, uint8 attrIndex, const glVertexAttr& attr) {
    o_assert_dbg(vaoSlotIndex < MaxNumSlots);
    o_assert_dbg(attrIndex < VertexAttr::NumVertexAttrs);
    this->glAttrs[vaoSlotIndex][attrIndex] = attr;
}

//------------------------------------------------------------------------------
void
glMesh::setInstanceMesh(const glMesh* msh) {
    o_assert_dbg((nullptr != msh) && (nullptr == this->instanceMesh));
    this->instanceMesh = msh;
}

} // namespace Render
} // namespace Oryol