//------------------------------------------------------------------------------
//  d3d11MeshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11MeshFactory.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11MeshFactory::d3d11MeshFactory() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
d3d11MeshFactory::~d3d11MeshFactory() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
d3d11MeshFactory::Setup(renderer* rendr, meshPool* mshPool) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
d3d11MeshFactory::Discard() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
bool
d3d11MeshFactory::IsValid() const {
    o_error("FIXME!\n");
    return false;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11MeshFactory::SetupResource(mesh& mesh) {
    o_error("FIXME!\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11MeshFactory::SetupResource(mesh& mesh, const void* data, int32 size) {
    o_error("FIXME!\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
d3d11MeshFactory::DestroyResource(mesh& mesh) {
    o_error("FIXME!\n");
}

} // namespace _priv
} // namespace Oryol