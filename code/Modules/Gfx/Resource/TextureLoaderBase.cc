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
class Locator
TextureLoaderBase::Locator() const {
    return this->setup.Locator;
}

} // namespace Oryol