//------------------------------------------------------------------------------
//  d3d12TextureBlock.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12TextureBlock.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
d3d12TextureBlock::Clear() {
    this->bindStage = ShaderStage::InvalidShaderStage;
    this->d3d12SRVDescriptor.Invalidate();
    textureBlockBase::Clear();
}

} // namespace _priv
} // namespace Oryol
