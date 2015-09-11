//------------------------------------------------------------------------------
//  TextureBlockLayout.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureBlockLayout.h"

namespace Oryol {

//------------------------------------------------------------------------------
TextureBlockLayout::TextureBlockLayout() :
numComps(0) {
    // empty
}

//------------------------------------------------------------------------------
void
TextureBlockLayout::Clear() {
    this->comps.Fill(Component());
    this->numComps = 0;
}

//------------------------------------------------------------------------------
TextureBlockLayout&
TextureBlockLayout::Add(const Component& comp) {
    this->comps[this->numComps++] = comp;
    return *this;
}

//------------------------------------------------------------------------------
TextureBlockLayout&
TextureBlockLayout::Add(const StringAtom& name, TextureType::Code type, int32 bindSlot) {
    this->comps[this->numComps++] = Component(name, type, bindSlot);
    return *this;
}

//------------------------------------------------------------------------------
int32
TextureBlockLayout::ComponentIndexForBindSlot(int32 bindSlot) const {
    for (int i = 0; i < this->numComps; i++) {
        if (this->comps[i].BindSlot == bindSlot) {
            return i;
        }
    }
    return InvalidIndex;
}

} // namespace Oryol
