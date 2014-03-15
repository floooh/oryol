//------------------------------------------------------------------------------
//  VertexLayout.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexLayout.h"

namespace Oryol {
namespace Render {

using namespace Core;

//------------------------------------------------------------------------------
VertexLayout::VertexLayout() :
num(0) {
    // empty
}

//------------------------------------------------------------------------------
void
VertexLayout::Clear() {
    this->num = 0;
}

//------------------------------------------------------------------------------
void
VertexLayout::Add(const VertexComponent& comp) {
    int32 num = this->GetNumComponents();
    o_assert(num < MaxNumVertexComponents);
    this->comps[this->num++] = comp;
}

//------------------------------------------------------------------------------
void
VertexLayout::Add(const StringAtom& name, VertexFormat::Code format) {
    this->Add(VertexComponent(name, format));
}

//------------------------------------------------------------------------------
bool
VertexLayout::Empty() const {
    return 0 == this->num;
}

//------------------------------------------------------------------------------
int32
VertexLayout::GetNumComponents() const {
    return this->num;
}

//------------------------------------------------------------------------------
const VertexComponent&
VertexLayout::GetComponent(int32 index) const {
    o_assert((index >= 0) && (index < MaxNumVertexComponents));
    return this->comps[index];
}

//------------------------------------------------------------------------------
int32
VertexLayout::GetByteSize() const {
    int32 byteSize = 0;
    for (int32 i = 0; i < this->num; i++) {
        byteSize += this->comps[i].GetByteSize();
    }
    return byteSize;
}

//------------------------------------------------------------------------------
int32
VertexLayout::GetComponentByteOffset(int32 index) const {
    o_assert((index >= 0) && (index < MaxNumVertexComponents));
    int32 byteOffset = 0;
    for (int32 i = 0; i < index; i++) {
        byteOffset += this->comps[i].GetByteSize();
    }
    return byteOffset;
}

} // namespace Render
} // namespace Oryol
