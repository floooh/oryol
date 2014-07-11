//------------------------------------------------------------------------------
//  VertexLayout.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexLayout.h"
#include "Core/Assert.h"

namespace Oryol {
namespace Render {

//------------------------------------------------------------------------------
void
VertexLayout::Clear() {
    this->numComps = 0;
    this->byteSize = 0;
    for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        this->attrCompIndices[i] = InvalidIndex;
    }
    for (int32 i = 0; i < MaxNumVertexComponents; i++) {
        this->byteOffsets[i] = 0;
    }
}
    
//------------------------------------------------------------------------------
VertexLayout::VertexLayout() {
    this->Clear();
}

//------------------------------------------------------------------------------
void
VertexLayout::Add(const VertexComponent& comp) {
    o_assert(this->numComps < MaxNumVertexComponents);
    this->comps[this->numComps] = comp;
    this->attrCompIndices[comp.Attr()] = this->numComps;
    this->byteOffsets[this->numComps] = this->byteSize;
    this->byteSize += comp.ByteSize();
    this->numComps++;
}

//------------------------------------------------------------------------------
void
VertexLayout::Add(VertexAttr::Code attr, VertexFormat::Code format) {
    this->Add(VertexComponent(attr, format));
}

} // namespace Render
} // namespace Oryol
