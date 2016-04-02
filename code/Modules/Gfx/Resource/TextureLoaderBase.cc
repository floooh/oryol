//------------------------------------------------------------------------------
//  TextureLoaderBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureLoaderBase.h"

namespace Oryol {

OryolClassImpl(TextureLoaderBase);

//------------------------------------------------------------------------------
TextureLoaderBase::TextureLoaderBase(const TextureSetup& setup_) :
setup(setup_) {
    // empty
}

//------------------------------------------------------------------------------
TextureLoaderBase::TextureLoaderBase(const TextureSetup& setup_, LoadedFunc loadedFunc) :
setup(setup_),
onLoaded(loadedFunc)
{
  // empty
}

//------------------------------------------------------------------------------
class Locator
TextureLoaderBase::Locator() const {
    return this->setup.Locator;
}

} // namespace Oryol