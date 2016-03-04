//------------------------------------------------------------------------------
//  VertexLayout.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexLayout.h"
#include "Core/Assertion.h"

namespace Oryol {

//------------------------------------------------------------------------------
VertexLayout&
VertexLayout::Clear() {
    this->StepFunction = VertexStepFunction::PerVertex;
    this->StepRate = 1;
    this->numComps = 0;
    this->byteSize = 0;
    this->attrCompIndices.Fill(InvalidIndex);
    this->byteOffsets.Fill(0);
    return *this;
}
    
//------------------------------------------------------------------------------
VertexLayout::VertexLayout() {
    this->Clear();
}

//------------------------------------------------------------------------------
VertexLayout&
VertexLayout::Add(const Component& comp) {
    o_assert_dbg(this->numComps < GfxConfig::MaxNumVertexLayoutComponents);
    o_assert_dbg(InvalidIndex == this->attrCompIndices[comp.Attr]);

    this->comps[this->numComps] = comp;
    this->attrCompIndices[comp.Attr] = this->numComps;
    this->byteOffsets[this->numComps] = this->byteSize;
    this->byteSize += comp.ByteSize();
    o_assert_dbg(this->byteSize < 128);
    this->numComps++;
    return *this;
}

//------------------------------------------------------------------------------
VertexLayout&
VertexLayout::Add(VertexAttr::Code attr, VertexFormat::Code format) {
    return this->Add(Component(attr, format));
}

//------------------------------------------------------------------------------
VertexLayout&
VertexLayout::UseInstancing() {
    this->StepFunction = VertexStepFunction::PerInstance;
    this->StepRate = 1;
    return *this;
}

} // namespace Oryol
