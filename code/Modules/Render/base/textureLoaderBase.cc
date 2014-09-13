//------------------------------------------------------------------------------
//  textureLoaderBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "textureLoaderBase.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
textureLoaderBase::textureLoaderBase() :
texFactory(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
textureLoaderBase::~textureLoaderBase() {
    o_assert(nullptr == this->texFactory);
}

//------------------------------------------------------------------------------
void
textureLoaderBase::onAttachToFactory(textureFactory* factory) {
    o_assert(nullptr == this->texFactory);
    o_assert(nullptr != factory);
    this->texFactory = factory;
}

//------------------------------------------------------------------------------
void
textureLoaderBase::onDetachFromFactory() {
    o_assert(nullptr != this->texFactory);
    this->texFactory = 0;
}

} // namespace _priv
} // namespace Oryol
