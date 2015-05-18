//------------------------------------------------------------------------------
//  d3d11ShaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11ShaderFactory.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11ShaderFactory::d3d11ShaderFactory() {
    Log::Info("d3d11ShaderFactory::d3d11ShaderFactory()\n");
}

//------------------------------------------------------------------------------
d3d11ShaderFactory::~d3d11ShaderFactory() {
    Log::Info("d3d11ShaderFactory::~d3d11ShaderFactory()\n");
}

//------------------------------------------------------------------------------
void
d3d11ShaderFactory::Setup() {
    Log::Info("d3d11ShaderFactory::Setup()\n");
}

//------------------------------------------------------------------------------
void
d3d11ShaderFactory::Discard() {
    Log::Info("d3d11ShaderFactory::Discard()\n");
}

//------------------------------------------------------------------------------
bool
d3d11ShaderFactory::IsValid() const {
    o_error("FIXME!\n");
    return false;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11ShaderFactory::SetupResource(shader& shd) {
    o_error("FIXME!\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
d3d11ShaderFactory::DestroyResource(shader& shd) {
    o_error("FIXME!\n");
}

} // namespace _priv
} // namespace Oryol
