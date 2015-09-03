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
    this->vs.Fill(texAndTarget());
    this->fs.Fill(texAndTarget());
}

} // namespace _priv
} // namespace Oryol
