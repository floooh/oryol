//------------------------------------------------------------------------------
//  IndexBufferAttrs.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IndexBufferAttrs.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
IndexBufferAttrs::IndexBufferAttrs() :
numIndices(0),
indexType(IndexType::InvalidIndexType),
usage(Usage::InvalidUsage) {
    // empty
}

//------------------------------------------------------------------------------
void
IndexBufferAttrs::setNumIndices(int32 num) {
    this->numIndices = num;
}

//------------------------------------------------------------------------------
void
IndexBufferAttrs::setIndexType(IndexType::Code t) {
    this->indexType = t;
}

//------------------------------------------------------------------------------
void
IndexBufferAttrs::setUsage(Usage::Code usg) {
    this->usage = usg;
}
    
} // namespace Render
} // namespace Oryol