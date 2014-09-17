//------------------------------------------------------------------------------
//  meshLoaderBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "meshLoaderBase.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
meshLoaderBase::meshLoaderBase() :
mshFactory(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
meshLoaderBase::~meshLoaderBase() {
    o_assert(nullptr == this->mshFactory);
}

//------------------------------------------------------------------------------
void
meshLoaderBase::onAttachToFactory(meshFactory* factory) {
    o_assert(nullptr == this->mshFactory);
    o_assert(nullptr != factory);
    this->mshFactory = factory;
}

//------------------------------------------------------------------------------
void
meshLoaderBase::onDetachFromFactory() {
    o_assert(nullptr != this->mshFactory);
    this->mshFactory = 0;
}
    
} // namespace _priv
} // namespace Oryol
