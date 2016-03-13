//------------------------------------------------------------------------------
//  vlkShaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlkShaderFactory.h"
#include "Gfx/Resource/shader.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkShaderFactory::~vlkShaderFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
vlkShaderFactory::Setup(const gfxPointers& ptr) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptr;
    o_warn("vlkShaderFactory::Setup() called\n");
}

//------------------------------------------------------------------------------
void
vlkShaderFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    o_warn("vlkShaderFactory::Discard() called\n");
}

//------------------------------------------------------------------------------
bool
vlkShaderFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
vlkShaderFactory::SetupResource(shader& shd) {
    o_assert_dbg(this->isValid);
    o_error("FIXME!\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
vlkShaderFactory::DestroyResource(shader& shd) {
    o_assert_dbg(this->isValid);
    o_error("FIXME!\n");
}

} // namespace _priv
} // namespace Oryol
