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
    for (int i = 0; i < this->numComps; i++) {
        this->comps[i].Clear();
    }
    this->numComps = 0;
    this->byteSize = 0;
}

//------------------------------------------------------------------------------
UniformLayout&
UniformLayout::Add(const Component& comp) {
    o_assert(this->numComps < MaxNumComponents);
    this->comps[this->numComps] = comp;
    this->byteOffsets[this->numComps] = this->byteSize;
    this->byteSize += comp.ByteSize();
    this->numComps++;
    return *this;
}

//------------------------------------------------------------------------------
UniformLayout&
UniformLayout::Add(const StringAtom& name, UniformType::Code type, uint8 num) {
    return this->Add(Component(name, type, num));
}

} // namespace Oryol
