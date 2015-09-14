//------------------------------------------------------------------------------
//  glTextureBlock.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glTextureBlock.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
glTextureBlock::Clear() {
    this->numEntries = 0;
    this->entries.Fill(entry());
    textureBlockBase::Clear();
}

} // namespace _priv
} // namespace Oryol
