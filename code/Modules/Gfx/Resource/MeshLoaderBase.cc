//------------------------------------------------------------------------------
//  MeshLoaderBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "MeshLoaderBase.h"

namespace Oryol {

OryolClassImpl(MeshLoaderBase);

//------------------------------------------------------------------------------
MeshLoaderBase::MeshLoaderBase(const MeshSetup& setup_, int32 ioLane_) :
setup(setup_),
ioLane(ioLane_) {
    // empty
}

//------------------------------------------------------------------------------
MeshLoaderBase::MeshLoaderBase(const MeshSetup& setup_, int32 ioLane_, LoadedFunc loadedFunc) :
setup(setup_),
ioLane(ioLane_),
onLoaded(loadedFunc) {
    // empty
}

//------------------------------------------------------------------------------
class Locator
MeshLoaderBase::Locator() const {
    return this->setup.Locator;
}

} // namespace Oryol