//------------------------------------------------------------------------------
//  vlkMeshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlkMeshFactory.h"
#include "Gfx/Resource/mesh.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkMeshFactory::~vlkMeshFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
vlkMeshFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkMeshFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
bool
vlkMeshFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
vlkMeshFactory::SetupResource(mesh& msh) {
    o_assert_dbg(this->isValid);
    o_error("FIXME!\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
ResourceState::Code
vlkMeshFactory::SetupResource(mesh& msh, const void* data, int32 size) {
    o_assert_dbg(this->isValid);
    o_error("FIXME!\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
vlkMeshFactory::DestroyResource(mesh& msh) {
    o_assert_dbg(this->isValid);
    o_error("FIXME!\n");
}

} // namespace _priv
} // namespace Oryol
