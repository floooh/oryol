//------------------------------------------------------------------------------
//  TextureLoaderBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureLoaderBase.h"

namespace Oryol {

OryolClassImpl(TextureLoaderBase);

//------------------------------------------------------------------------------
TextureLoaderBase::TextureLoaderBase(const TextureSetup& setup_, int32 ioLane_) :
setup(setup_),
ioLane(ioLane_) {
    // empty
}

//------------------------------------------------------------------------------
const class Locator&
TextureLoaderBase::Locator() const {
    return this->setup.Locator;
}

} // namespace Oryol