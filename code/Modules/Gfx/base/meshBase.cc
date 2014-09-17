//------------------------------------------------------------------------------
//  meshBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "meshBase.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
meshBase::meshBase() :
numPrimitiveGroups(0) {
    // empty
}

//------------------------------------------------------------------------------
void
meshBase::clear() {
    this->ioRequest.Invalidate();
    this->vertexBufferAttrs = VertexBufferAttrs();
    this->indexBufferAttrs = IndexBufferAttrs();
    for (int32 i = 0; i < this->numPrimitiveGroups; i++) {
        this->primitiveGroups[i] = PrimitiveGroup();
    }
    this->numPrimitiveGroups = 0;
    resourceBase::clear();    
}

//------------------------------------------------------------------------------
void
meshBase::setIORequest(const Ptr<IOProtocol::Request>& req) {
    this->ioRequest = req;
}

//------------------------------------------------------------------------------
void
meshBase::setVertexBufferAttrs(const VertexBufferAttrs& attrs) {
    this->vertexBufferAttrs = attrs;
}

//------------------------------------------------------------------------------
void
meshBase::setIndexBufferAttrs(const IndexBufferAttrs& attrs) {
    this->indexBufferAttrs = attrs;
}

//------------------------------------------------------------------------------
void
meshBase::setNumPrimitiveGroups(int32 num) {
    o_assert((num > 0) && (num <= MaxNumPrimitiveGroups));
    this->numPrimitiveGroups = num;
}

//------------------------------------------------------------------------------
void
meshBase::setPrimitiveGroup(int32 index, const PrimitiveGroup& group) {
    o_assert_range(index, MaxNumPrimitiveGroups);
    this->primitiveGroups[index] = group;
}

} // namespace _priv
} // namespace Oryol