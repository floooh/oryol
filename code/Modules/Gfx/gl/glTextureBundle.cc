//------------------------------------------------------------------------------
//  glTextureBundle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glTextureBundle.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
glTextureBundle::Clear() {
    this->vs.Fill(bindEntry());
    this->fs.Fill(bindEntry());
    textureBundleBase::Clear();
}

} // namespace _priv
} // namespace Oryol
