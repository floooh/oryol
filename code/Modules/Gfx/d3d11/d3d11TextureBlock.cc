//------------------------------------------------------------------------------
//  d3d11TextureBlock.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11TextureBlock.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
d3d11TextureBlock::Clear() {
    this->bindStage = ShaderStage::InvalidShaderStage;
    this->entries.Fill(entry());
    textureBlockBase::Clear();
}

} // namespace _priv
} // namespace Oryol
