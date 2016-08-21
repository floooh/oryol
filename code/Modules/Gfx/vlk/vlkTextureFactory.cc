//------------------------------------------------------------------------------
//   vlkTextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlkTextureFactory.h"
#include "Core/Assertion.h"
#include "Gfx/Resource/resource.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkTextureFactory::~vlkTextureFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
vlkTextureFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
    o_warn("vlkTextureFactory::Setup() called\n");
}

//------------------------------------------------------------------------------
void
vlkTextureFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    o_warn("vlkTextureFactory::Discard() called\n");
}

//------------------------------------------------------------------------------
bool
vlkTextureFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
vlkTextureFactory::SetupResource(texture& tex) {
    o_assert_dbg(this->isValid);
    o_error("FIXME!\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
ResourceState::Code
vlkTextureFactory::SetupResource(texture& tex, const void* data, int32 size) {
    o_assert_dbg(this->isValid);
    o_error("FIXME!\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
vlkTextureFactory::DestroyResource(texture& tex) {
    o_assert_dbg(this->isValid);
    o_error("FIXME!\n");
}

} // namespace _priv
} // namespace Oryol
