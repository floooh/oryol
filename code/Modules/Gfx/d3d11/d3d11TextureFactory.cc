//------------------------------------------------------------------------------
//  d3d11TextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11TextureFactory.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11TextureFactory::d3d11TextureFactory() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
d3d11TextureFactory::~d3d11TextureFactory() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
d3d11TextureFactory::Setup(class renderer* rendr, displayMgr* displayMgr, texturePool* texPool) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
d3d11TextureFactory::Discard() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
bool
d3d11TextureFactory::IsValid() const {
    o_error("FIXME!\n");
    return false;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11TextureFactory::SetupResource(texture& tex) {
    o_error("FIXME!\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11TextureFactory::SetupResource(texture& tex, const void* data, int32 size) {
    o_error("FIXME!\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
d3d11TextureFactory::DestroyResource(texture& tex) {
    o_error("FIXME!\n");
}

} // namespace _priv
} // namespace Oryol