//------------------------------------------------------------------------------
//  VertexLayout.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexLayout.h"
#include "Core/Assertion.h"
#include "Core/Hash/fasthash.h"

namespace Oryol {

using namespace _priv;

//------------------------------------------------------------------------------
VertexLayout&
VertexLayout::Clear() {
    this->StepFunction = VertexStepFunction::PerVertex;
    this->StepRate = 0;
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
uint64
VertexLayout::CombinedHash(const VertexLayout& l0, const VertexLayout& l1) {
    // returns a hash of two combined vertex layouts, this is used
    // for finding matching mesh/vertexshader pairs
    StaticArray<Component, GfxConfig::MaxNumVertexLayoutComponents * 2> combined;
    int i = 0;
    for (const auto& comp : l0.comps) {
        combined[i++] = comp;
    }
    for (const auto& comp : l1.comps) {
        combined[i++] = comp;
    }
    const uint64 seed = 0xABADFEED12345678;
    uint64 hash = fasthash64(&combined[0], combined.Size() * sizeof(Component), seed);
    return hash;    
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
