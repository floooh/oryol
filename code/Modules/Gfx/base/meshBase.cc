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
meshBase::clear() {
    this->vertexBufferAttrs = VertexBufferAttrs();
    this->indexBufferAttrs = IndexBufferAttrs();
    for (int32 i = 0; i < MaxNumPrimGroups; i++) {
        this->primGroup[i] = PrimitiveGroup();
    }
    this->numPrimGroups = 0;
    resourceBase::clear();    
}

} // namespace _priv
} // namespace Oryol