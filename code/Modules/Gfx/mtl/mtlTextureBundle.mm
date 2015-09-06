//------------------------------------------------------------------------------
//  mtlTextureBundle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlTextureBundle.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlTextureBundle::entry::entry() :
bindSlotIndex(InvalidIndex),
mtlTex(nil),
mtlSamplerState(nil) {
    // empty
}

//------------------------------------------------------------------------------
void
mtlTextureBundle::Clear() {
    this->vs.Fill(entry());
    this->fs.Fill(entry());
    textureBundleBase::Clear();
}

} // namespace _priv
} // namespace Oryol
