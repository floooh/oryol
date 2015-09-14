//------------------------------------------------------------------------------
//  mtlTextureBlock.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlTextureBlock.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlTextureBlock::entry::entry() :
bindSlot(InvalidIndex),
mtlTex(nil),
mtlSamplerState(nil) {
    // empty
}

//------------------------------------------------------------------------------
void
mtlTextureBlock::Clear() {
    this->bindStage = ShaderStage::InvalidShaderStage;
    this->numEntries = 0;
    this->entries.Fill(entry());
    textureBlockBase::Clear();
}

} // namespace _priv
} // namespace Oryol
