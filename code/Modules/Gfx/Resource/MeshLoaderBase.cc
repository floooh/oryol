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
const class Locator&
MeshLoaderBase::Locator() const {
    return this->setup.Locator;
}

} // namespace Oryol