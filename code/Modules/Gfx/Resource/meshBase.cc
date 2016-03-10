//------------------------------------------------------------------------------
//  meshBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "meshBase.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
meshBase::meshBase() :
numPrimGroups(0) {
    // empty
}

//------------------------------------------------------------------------------
void
meshBase::Clear() {
    this->vertexBufferAttrs = VertexBufferAttrs();
    this->indexBufferAttrs = IndexBufferAttrs();
    this->primGroups.Fill(PrimitiveGroup());
    this->numPrimGroups = 0;
    resourceBase::Clear();
}

} // namespace _priv
} // namespace Oryol
