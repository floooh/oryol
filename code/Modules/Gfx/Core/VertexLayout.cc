//------------------------------------------------------------------------------
//  VertexLayout.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexLayout.h"
#include "Core/Assert.h"

namespace Oryol {

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
VertexLayout&
VertexLayout::Add(const VertexComponent& comp) {
    o_assert(this->numComps < MaxNumVertexComponents);
    this->comps[this->numComps] = comp;
    this->attrCompIndices[comp.Attr] = this->numComps;
    this->byteOffsets[this->numComps] = this->byteSize;
    this->byteSize += comp.ByteSize();
    this->numComps++;
    return *this;
}

//------------------------------------------------------------------------------
VertexLayout&
VertexLayout::Add(VertexAttr::Code attr, VertexFormat::Code format) {
    return this->Add(VertexComponent(attr, format));
}

} // namespace Oryol
