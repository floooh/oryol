//------------------------------------------------------------------------------
//  d3d12TextureBlockFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12TextureBlockFactory.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12TextureBlockFactory::d3d12TextureBlockFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
d3d12TextureBlockFactory::~d3d12TextureBlockFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d12TextureBlockFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
d3d12TextureBlockFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
bool
d3d12TextureBlockFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12TextureBlockFactory::SetupResource(textureBlock& tb) {
    o_assert_dbg(this->isValid);
    o_warn("FIXME!\n");
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d12TextureBlockFactory::DestroyResource(textureBlock& tb) {
    o_assert_dbg(this->isValid);
    o_warn("FIXME!\n");
}

} // namespace _priv
} // namespace Oryol
