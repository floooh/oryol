//------------------------------------------------------------------------------
//  UniformBlockLayout.h
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UniformBlockLayout.h"

namespace Oryol {

//------------------------------------------------------------------------------
UniformBlockLayout::UniformBlockLayout() :
TypeHash(0),
numComps(0),
byteSize(0) {
    // empty
}

//------------------------------------------------------------------------------
void
UniformBlockLayout::Clear() {
    this->comps.Fill(Component());
    this->byteOffsets.Fill(0);
    this->numComps = 0;
    this->byteSize = 0;
}

//------------------------------------------------------------------------------
UniformBlockLayout&
UniformBlockLayout::Add(const Component& comp) {
    this->comps[this->numComps] = comp;
    this->byteOffsets[this->numComps] = this->byteSize;
    this->byteSize += comp.ByteSize();
    this->numComps++;
    return *this;
}

//------------------------------------------------------------------------------
UniformBlockLayout&
UniformBlockLayout::Add(const StringAtom& name, UniformType::Code type) {
    return this->Add(Component(name, type));
}

//------------------------------------------------------------------------------
UniformBlockLayout&
UniformBlockLayout::Add(const StringAtom& name, UniformType::Code type, int32 numElements) {
    return this->Add(Component(name, type, numElements));
}

} // namespace Oryol
