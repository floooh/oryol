//------------------------------------------------------------------------------
//  UniformLayout.h
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UniformLayout.h"

namespace Oryol {

//------------------------------------------------------------------------------
UniformLayout::UniformLayout() :
TypeHash(0),
numComps(0),
byteSize(0) {
    // empty
}

//------------------------------------------------------------------------------
void
UniformLayout::Clear() {
    this->comps.Fill(Component());
    this->byteOffsets.Fill(0);
    this->numComps = 0;
    this->byteSize = 0;
}

//------------------------------------------------------------------------------
UniformLayout&
UniformLayout::Add(const Component& comp) {
    o_assert(this->numComps < GfxConfig::MaxNumUniformLayoutComponents);
    this->comps[this->numComps] = comp;
    this->byteOffsets[this->numComps] = this->byteSize;
    this->byteSize += comp.ByteSize();
    this->numComps++;
    return *this;
}

//------------------------------------------------------------------------------
UniformLayout&
UniformLayout::Add(const StringAtom& name, UniformType::Code type) {
    return this->Add(Component(name, type));
}

} // namespace Oryol
