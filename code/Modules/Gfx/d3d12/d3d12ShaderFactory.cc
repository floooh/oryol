//------------------------------------------------------------------------------
//  d3d12ShaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12ShaderFactory.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12ShaderFactory::d3d12ShaderFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
d3d12ShaderFactory::~d3d12ShaderFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d12ShaderFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
d3d12ShaderFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
bool
d3d12ShaderFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12ShaderFactory::SetupResource(shader& shd) {
    o_assert_dbg(this->isValid);
    o_warn("d3d12ShaderFactory::SetupResource()\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
d3d12ShaderFactory::DestroyResource(shader& shd) {
    o_assert_dbg(this->isValid);
    o_warn("d3d12ShaderFactory::DestroyResource()\n");
}

} // namespace _priv
} // namespace Oryol

