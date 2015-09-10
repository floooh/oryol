//------------------------------------------------------------------------------
//  d3d11TextureBundle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11TextureBundle.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
d3d11TextureBundle::Clear() {
    this->vs.Fill(entry());
    this->fs.Fill(entry());
    textureBundleBase::Clear();
}

} // namespace _priv
} // namespace Oryol
