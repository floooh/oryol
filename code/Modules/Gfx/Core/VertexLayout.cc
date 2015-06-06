//------------------------------------------------------------------------------
//  VertexLayout.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexLayout.h"
#include "Core/Assertion.h"

namespace Oryol {

//------------------------------------------------------------------------------
void
VertexLayout::Clear() {
    this->numComps = 0;
    this->byteSize = 0;
    this->attrCompIndices.Fill(InvalidIndex);
    this->byteOffsets.Fill(0);
}
    
//------------------------------------------------------------------------------
VertexLayout::VertexLayout() {
    this->Clear();
}

//------------------------------------------------------------------------------
VertexLayout&
VertexLayout::Add(const Component& comp) {
    o_assert(this->numComps < MaxNumComponents);
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
    return this->Add(Component(attr, format));
}

} // namespace Oryol
