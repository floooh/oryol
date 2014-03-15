//------------------------------------------------------------------------------
//  glMesh.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glMesh.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
glMesh::glMesh() {
    this->glVertexBuffer = 0;
    this->glIndexBuffer = 0;
    this->glVertexArrayObject = 0;
}

//------------------------------------------------------------------------------
glMesh::~glMesh() {
    o_assert(0 == this->glVertexBuffer);
    o_assert(0 == this->glIndexBuffer);
    o_assert(0 == this->glVertexArrayObject);
}


//------------------------------------------------------------------------------
void
glMesh::clear() {
    this->glVertexBuffer = 0;
    this->glIndexBuffer = 0;
    this->glVertexArrayObject = 0;
    meshBase::clear();
}

//------------------------------------------------------------------------------
void
glMesh::glSetVertexBuffer(GLuint vb) {
    o_assert(0 == this->glVertexBuffer);
    this->glVertexBuffer = vb;
}

//------------------------------------------------------------------------------
void
glMesh::glSetIndexBuffer(GLuint ib) {
    o_assert(0 == this->glIndexBuffer);
    this->glIndexBuffer = ib;
}

//------------------------------------------------------------------------------
void
glMesh::glSetVertexArrayObject(GLuint vao) {
    o_assert(0 == this->glVertexArrayObject);
    this->glVertexArrayObject = vao;
}

} // namespace Render
} // namespace Oryol