#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glMesh
    @ingroup _priv
    @brief GL implement of Mesh
*/
#include "Core/Containers/StaticArray.h"
#include "Gfx/Resource/meshBase.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/gl/gl_decl.h"
#include "Gfx/gl/glVertexAttr.h"

namespace Oryol {
namespace _priv {
    
class glMesh : public meshBase {
public:
    /// constructor
    glMesh();
    /// destructor
    ~glMesh();

    static const int32 MaxNumSlots = 4;

    /// clear the object
    void Clear();
    
    /// number of vertex buffer slots
    uint8 numVertexBufferSlots;
    /// active vertex buffer slot
    uint8 activeVertexBufferSlot;
    /// GL vertex buffers
    StaticArray<GLuint, MaxNumSlots> glVertexBuffers;
    
    /// number of vertex array object slots
    uint8 numVAOSlots;
    /// set active vertex array object slot
    void setActiveVAOSlot(uint8 s);
    /// get active vertex array object slot
    uint8 getActiveVAOSlot() const;
    /// GL vertex array objects
    StaticArray<GLuint, MaxNumSlots> glVAOs;
    /// GL index buffer
    GLuint glIndexBuffer;

    /// GL vertex attributes
    StaticArray<StaticArray<glVertexAttr, VertexAttr::NumVertexAttrs>, MaxNumSlots> glAttrs;

    /// optional instance data mesh
    const glMesh* instanceMesh;
    
private:
    uint8 activeVAOSlot;
};

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
        return this->instanceMesh->activeVertexBufferSlot;
    }
    else {
        return this->activeVAOSlot;
    }
}

} // namespace _priv
} // namespace Oryol
