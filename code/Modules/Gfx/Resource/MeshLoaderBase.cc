//------------------------------------------------------------------------------
//  MeshLoaderBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "MeshLoaderBase.h"

namespace Oryol {

OryolClassImpl(MeshLoaderBase);

//------------------------------------------------------------------------------
bool
MeshLoaderBase::Supports(GfxResourceType::Code resType) const {
    return GfxResourceType::Mesh == resType;
}

//------------------------------------------------------------------------------
bool
MeshLoaderBase::Accepts(const SetupAndStream<MeshSetup>& input) const {
    return false;
}

//------------------------------------------------------------------------------
SetupAndStream<MeshSetup>
MeshLoaderBase::Load(const SetupAndStream<MeshSetup>& input) const {
    return input;
}

} // namespace Oryol