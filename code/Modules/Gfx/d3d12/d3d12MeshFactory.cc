//------------------------------------------------------------------------------
//  d3d12MeshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12MeshFactory.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12MeshFactory::d3d12MeshFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
d3d12MeshFactory::~d3d12MeshFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d12MeshFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
d3d12MeshFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->pointers = gfxPointers();
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
d3d12MeshFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12MeshFactory::SetupResource(mesh& msh) {
    o_assert_dbg(this->isValid);
    o_warn("d3d12MeshFactory::SetupResource()\n");
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12MeshFactory::SetupResource(mesh& msh, const void* data, int32 size) {
    o_assert_dbg(this->isValid);
    o_warn("d3d12MeshFactory::SetupResource()\n");
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d12MeshFactory::DestroyResource(mesh& msh) {
    o_assert_dbg(this->isValid);
    o_warn("d3d12MeshFactory::DestroyResource()\n");
}

} // namespace _priv
} // namespace Oryol
