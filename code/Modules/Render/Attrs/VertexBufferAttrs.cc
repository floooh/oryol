//------------------------------------------------------------------------------
//  VertexBufferAttrs.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexBufferAttrs.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
VertexBufferAttrs::VertexBufferAttrs() :
numVertices(0),
usage(Usage::InvalidUsage) {
    // empty
}

//------------------------------------------------------------------------------
void
VertexBufferAttrs::setNumVertices(int32 num) {
    o_assert(num > 0);
    this->numVertices = num;
}

//------------------------------------------------------------------------------
void
VertexBufferAttrs::setVertexLayout(const VertexLayout& vl) {
    this->vertexLayout = vl;
}

//------------------------------------------------------------------------------
void
VertexBufferAttrs::setUsage(Usage::Code usg) {
    this->usage = usg;
}

} // namespace Render
} // namespace Oryol