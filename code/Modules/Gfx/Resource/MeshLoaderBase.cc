//------------------------------------------------------------------------------
//  MeshLoaderBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "MeshLoaderBase.h"

namespace Oryol {

//------------------------------------------------------------------------------
MeshLoaderBase::MeshLoaderBase(const MeshSetup& setup_) :
setup(setup_) {
    // empty
}

//------------------------------------------------------------------------------
MeshLoaderBase::MeshLoaderBase(const MeshSetup& setup_, LoadedFunc loadedFunc) :
setup(setup_),
onLoaded(loadedFunc) {
    // empty
}

//------------------------------------------------------------------------------
class Locator
MeshLoaderBase::Locator() const {
    return this->setup.Locator;
}

} // namespace Oryol