//------------------------------------------------------------------------------
//  d3d12TextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12TextureFactory.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12TextureFactory::d3d12TextureFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
d3d12TextureFactory::~d3d12TextureFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d12TextureFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
d3d12TextureFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
bool
d3d12TextureFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12TextureFactory::SetupResource(texture& tex) {
    o_assert_dbg(this->isValid);
    o_warn("d3d12TextureFactory::SetupResource()\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12TextureFactory::SetupResource(texture& tex, const void* data, int32 size) {
    o_assert_dbg(this->isValid);
    o_warn("d3d12TextureState::SetupResource()\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
d3d12TextureFactory::DestroyResource(texture& tex) {
    o_assert_dbg(this->isValid);
    o_warn("d3d12TextureFacgtory::DestroyResource()\n");
}

} // namespace _priv
} // namespace Oryol
