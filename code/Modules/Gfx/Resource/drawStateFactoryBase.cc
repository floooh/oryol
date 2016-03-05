//------------------------------------------------------------------------------
//  drawStateFactoryBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "drawStateFactoryBase.h"
#include "Gfx/Resource/drawState.h"
#include "Gfx/Resource/resourcePools.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
drawStateFactoryBase::drawStateFactoryBase() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
drawStateFactoryBase::~drawStateFactoryBase() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
drawStateFactoryBase::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->pointers = ptrs;
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
drawStateFactoryBase::Discard() {
    o_assert_dbg(this->isValid);
    this->pointers = gfxPointers();
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
drawStateFactoryBase::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
drawStateFactoryBase::SetupResource(drawState& ds) {
    o_assert_dbg(this->isValid);
    ds.shd = this->pointers.shaderPool->Lookup(ds.Setup.Shader);
    o_assert_dbg(ds.shd && (ResourceState::Valid == ds.shd->State));
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
drawStateFactoryBase::DestroyResource(drawState& ds) {
    ds.Clear();
}

} // namespace _priv
} // namespace Oryol
