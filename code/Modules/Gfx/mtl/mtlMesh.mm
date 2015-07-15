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
activeVertexBufferSlot(0),
mtlIndexBuffer(nil) {
    this->mtlVertexBuffers.Fill(nil);
}

//------------------------------------------------------------------------------
mtlMesh::~mtlMesh() {
    o_assert_dbg(nil == this->mtlVertexBuffers[0]);
    o_assert_dbg(nil == this->mtlIndexBuffer);
}

//------------------------------------------------------------------------------
void
mtlMesh::Clear() {
    this->activeVertexBufferSlot = 0;
    this->mtlVertexBuffers.Fill(nil);
    this->mtlIndexBuffer = nil;
    meshBase::Clear();
}

} // namespace _priv
} // namespace Oryol