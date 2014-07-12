#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glMesh
    @brief GL implement of Mesh
*/
#include "Render/base/meshBase.h"
#include "Render/Core/Enums.h"
#include "Render/gl/gl_decl.h"
#include "Render/gl/glVertexAttr.h"

namespace Oryol {
namespace Render {
    
class glMesh : public meshBase {
public:
    /// constructor
    glMesh();
    /// destructor
    ~glMesh();

    /// clear the object
    void clear();
    
    /// set number of vertex buffer slots
    void setNumVertexBufferSlots(uint8 num);
    /// get number of vertex buffer slots
    uint8 getNumVertexBufferSlots() const;
    /// set active vertex buffer slot
    void setActiveVertexBufferSlot(uint8 index);
    /// get active vertex buffer slot
    uint8 getActiveVertexBufferSlot() const;
    /// set vertex buffer at slot index
    void glSetVertexBuffer(uint8 index, GLuint vb);
    /// get GL vertex buffer
    GLuint glGetVertexBuffer(uint8 index) const;
    
    /// set number of vertex array object slots
    void setNumVAOSlots(uint8 num);
    /// get number of vertex array object slots
    uint8 getNumVAOSlots() const;
    /// set active vertex array object slot
    void setActiveVAOSlot(uint8 index);
    /// get active vertex array object slot
    uint8 getActiveVAOSlot() const;
    /// set GL vertex array object at slot
    void glSetVAO(uint8 index, GLuint vao);
    /// get GL vertex array object at slot (can be 0
    GLuint glGetVAO(uint8 index) const;

    /// set optional GL index buffer
    void glSetIndexBuffer(GLuint ib);
    /// get optional GL index buffer
    GLuint glGetIndexBuffer() const;
    
    /// set glVertexAttr object at index
    void glSetAttr(uint8 vaoSlotIndex, uint8 attrIndex, const glVertexAttr& attr);
    /// get glVertexAttr object at index (read/write)
    glVertexAttr& glAttr(uint8 vaoSlotIndex, uint8 attrIndex);
    /// get glVertexAttr object (read only)
    const glVertexAttr& glAttr(uint8 vaoSlotIndex, uint8 attrIndex) const;

    /// set the optional instance data mesh
    void setInstanceMesh(const glMesh* msh);
    /// get the optional instance data mesh
    const glMesh* getInstanceMesh() const;
    
protected:
    static const int32 MaxNumSlots = 4;
    
    uint8 numVertexBufferSlots;
    uint8 activeVertexBufferSlot;
    uint8 numVAOSlots;
    uint8 activeVAOSlot;
    GLuint glVertexBuffer[MaxNumSlots];
    GLuint glVAO[MaxNumSlots];
    GLuint glIndexBuffer;
    const glMesh* instanceMesh;
    class glVertexAttr glAttrs[MaxNumSlots][VertexAttr::NumVertexAttrs];
};

//------------------------------------------------------------------------------
inline void
glMesh::setNumVertexBufferSlots(uint8 num) {
    o_assert_dbg(num <= MaxNumSlots);
    this->numVertexBufferSlots = num;
}

//------------------------------------------------------------------------------
inline uint8
glMesh::getNumVertexBufferSlots() const {
    return this->numVertexBufferSlots;
}

//------------------------------------------------------------------------------
inline void
glMesh::setActiveVertexBufferSlot(uint8 index) {
    o_assert_dbg(index < MaxNumSlots);
    this->activeVertexBufferSlot = index;
}

//------------------------------------------------------------------------------
inline uint8
glMesh::getActiveVertexBufferSlot() const {
    return this->activeVertexBufferSlot;
}

//------------------------------------------------------------------------------
inline void
glMesh::setNumVAOSlots(uint8 num) {
    o_assert_dbg(num <= MaxNumSlots);
    this->numVAOSlots = num;
}

//------------------------------------------------------------------------------
inline uint8
glMesh::getNumVAOSlots() const {
    return this->numVAOSlots;
}

//------------------------------------------------------------------------------
inline void
glMesh::setActiveVAOSlot(uint8 index) {
    // if this is the geometry mesh of an instancing mesh pair,
    // then our vertex-array-object slot is always the
    // active vertex-buffer slot of the instance data mesh
    o_assert_dbg(nullptr == this->instanceMesh);
    o_assert_dbg(index < MaxNumSlots);
    this->activeVAOSlot = index;
}

//------------------------------------------------------------------------------
inline uint8
glMesh::getActiveVAOSlot() const {
    // if this is the geometry mesh of an instancing mesh pair,
    // then our vertex-array-object slot is always the
    // active vertex-buffer slot of the instance data mesh
    if (this->instanceMesh) {
        return this->instanceMesh->getActiveVertexBufferSlot();
    }
    else {
        return this->activeVAOSlot;
    }
}

//------------------------------------------------------------------------------
inline GLuint
glMesh::glGetVertexBuffer(uint8 index) const {
    o_assert_dbg(index < MaxNumSlots);
    return this->glVertexBuffer[index];
}

//------------------------------------------------------------------------------
inline GLuint
glMesh::glGetVAO(uint8 index) const {
    o_assert_dbg(index < MaxNumSlots);
    return this->glVAO[index];
}
    
//------------------------------------------------------------------------------
inline GLuint
glMesh::glGetIndexBuffer() const {
    return this->glIndexBuffer;
}

//------------------------------------------------------------------------------
inline glVertexAttr&
glMesh::glAttr(uint8 vaoSlotIndex, uint8 attrIndex) {
    o_assert_dbg(vaoSlotIndex < MaxNumSlots);
    o_assert_dbg(attrIndex < VertexAttr::NumVertexAttrs);
    return this->glAttrs[vaoSlotIndex][attrIndex];
}

//------------------------------------------------------------------------------
inline const glVertexAttr&
glMesh::glAttr(uint8 vaoSlotIndex, uint8 attrIndex) const {
    o_assert_dbg(vaoSlotIndex < MaxNumSlots);
    o_assert_dbg(attrIndex < VertexAttr::NumVertexAttrs);
    return this->glAttrs[vaoSlotIndex][attrIndex];
}

//------------------------------------------------------------------------------
inline const glMesh*
glMesh::getInstanceMesh() const {
    return this->instanceMesh;
}

} // namespace Oryol
} // namespace Render