#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glMesh
    @brief GL implement of Mesh
*/
#include "Render/base/meshBase.h"
#include "Render/gl/gl.h"

namespace Oryol {
namespace Render {
    
class glMesh : public meshBase {
public:
    /// constructor
    glMesh();
    /// destructor
    ~glMesh();
    
    /// get GL vertex buffer
    GLuint glGetVertexBuffer() const;
    /// get GL index buffer (can be 0)
    GLuint glGetIndexBuffer() const;
    /// get GL vertex array object (0 if not supported)
    GLuint glGetVertexArrayObject() const;
    
protected:
    friend class glMeshFactory;
    /// clear the object
    void clear();
    /// set GL vertex buffer
    void glSetVertexBuffer(GLuint vb);
    /// set GL index buffer
    void glSetIndexBuffer(GLuint ib);
    /// set GL vertex array object
    void glSetVertexArrayObject(GLuint vao);
    
    GLuint glVertexBuffer;
    GLuint glIndexBuffer;
    GLuint glVertexArrayObject;
};

//------------------------------------------------------------------------------
inline GLuint
glMesh::glGetVertexBuffer() const {
    return this->glVertexBuffer;
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

} // namespace Oryol
} // namespace Render