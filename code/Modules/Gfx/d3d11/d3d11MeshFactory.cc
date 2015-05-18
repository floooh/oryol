//------------------------------------------------------------------------------
//  d3d11MeshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11MeshFactory.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11MeshFactory::d3d11MeshFactory() {
    Log::Info("d3d11MeshFactory::d3d11MeshFactory()\n");
}

//------------------------------------------------------------------------------
d3d11MeshFactory::~d3d11MeshFactory() {
    Log::Info("d3d11MeshFactory::~d3d11MeshFactory()\n");
}

//------------------------------------------------------------------------------
void
d3d11MeshFactory::Setup(renderer* rendr, meshPool* mshPool) {
    Log::Info("d3d11MeshFactory::Setup()\n");
}

//------------------------------------------------------------------------------
void
d3d11MeshFactory::Discard() {
    Log::Info("d3d11MeshFactory::Discard()\n");
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