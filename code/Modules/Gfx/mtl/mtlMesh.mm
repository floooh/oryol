//------------------------------------------------------------------------------
//  mtlMesh.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlMesh.h"
#include "mtl_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlMesh::mtlMesh() :
mtlVertexBuffer(nil),
mtlIndexBuffer(nil) {
    // empty
}

//------------------------------------------------------------------------------
mtlMesh::~mtlMesh() {
    o_assert_dbg(nil == this->mtlVertexBuffer);
    o_assert_dbg(nil == this->mtlIndexBuffer);
}

//------------------------------------------------------------------------------
void
mtlMesh::Clear() {
    this->mtlVertexBuffer = nil;
    this->mtlIndexBuffer = nil;
    meshBase::Clear();
}

} // namespace _priv
} // namespace Oryol