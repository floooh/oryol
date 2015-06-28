//------------------------------------------------------------------------------
//  mtlMeshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlMeshFactory.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlMeshFactory::mtlMeshFactory() :
renderer(nullptr),
meshPool(nullptr),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
mtlMeshFactory::~mtlMeshFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
mtlMeshFactory::Setup(class renderer* rendr, class meshPool* mshPool) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(nullptr != rendr);
    o_assert_dbg(nullptr != mshPool);
    this->isValid = true;
    this->renderer = rendr;
    this->meshPool = mshPool;
}

//------------------------------------------------------------------------------
void
mtlMeshFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->renderer = nullptr;
    this->meshPool = nullptr;
}

//------------------------------------------------------------------------------
bool
mtlMeshFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlMeshFactory::SetupResource(mesh& msh) {
    o_assert_dbg(this->isValid);

    o_error("IMPLEMENT ME: mtlMeshFactory::SetupResource()\n");

    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlMeshFactory::SetupResource(mesh& msh, const void* data, int32 size) {
    o_assert_dbg(this->isValid);

    o_error("IMPLEMENT ME: mtlMeshFactory::SetupResource()\n");

    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
mtlMeshFactory::DestroyResource(mesh& mesh) {
    o_error("IMPLEMENT ME: mtlMeshFactory::DestroyResource()!\n");
}

} // namespace _priv
} // namespace Oryol
