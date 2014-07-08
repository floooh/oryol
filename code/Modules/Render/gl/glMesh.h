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
    
    /// set GL vertex buffer
    void glSetVertexBuffer(GLuint vb);
    /// get GL vertex buffer
    GLuint glGetVertexBuffer() const;
    /// set optional GL instance data vertex buffer
    void glSetInstanceBuffer(GLuint ivb);
    /// get optional GL instance data vertex buffer (0 if not set)
    GLuint glGetInstanceBuffer() const;
    /// set GL index buffer
    void glSetIndexBuffer(GLuint ib);
    /// get GL index buffer (can be 0)
    GLuint glGetIndexBuffer() const;
    /// set GL vertex array object
    void glSetVertexArrayObject(GLuint vao);
    /// get GL vertex array object (0 if not supported)
    GLuint glGetVertexArrayObject() const;
    /// set glVertexAttr object at index
    void glSetAttr(int32 index, const glVertexAttr& attr);
    /// get glVertexAttr object at index (read/write)
    glVertexAttr& glAttr(int32 index);
    /// get glVertexAttr object (read only)
    const glVertexAttr& glAttr(int32 index) const;
    
protected:
    GLuint glVertexBuffer;
    GLuint glInstanceBuffer;
    GLuint glIndexBuffer;
    GLuint glVertexArrayObject;
    class glVertexAttr glAttrs[VertexAttr::NumVertexAttrs];
};

//------------------------------------------------------------------------------
inline GLuint
glMesh::glGetVertexBuffer() const {
    return this->glVertexBuffer;
}

//------------------------------------------------------------------------------
inline GLuint
glMesh::glGetInstanceBuffer() const {
    return this->glInstanceBuffer;
}

//------------------------------------------------------------------------------
inline GLuint
glMesh::glGetIndexBuffer() const {
    return this->glIndexBuffer;
}

//------------------------------------------------------------------------------
inline GLuint
glMesh::glGetVertexArrayObject() const {
    return this->glVertexArrayObject;
}

//------------------------------------------------------------------------------
inline glVertexAttr&
glMesh::glAttr(int32 index) {
    o_assert_range_dbg(index, VertexAttr::NumVertexAttrs);
    return this->glAttrs[index];
}

//------------------------------------------------------------------------------
inline const glVertexAttr&
glMesh::glAttr(int32 index) const {
    o_assert_range_dbg(index, VertexAttr::NumVertexAttrs);
    return this->glAttrs[index];
}

} // namespace Oryol
} // namespace Render