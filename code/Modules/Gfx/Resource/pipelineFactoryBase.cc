//------------------------------------------------------------------------------
//  pipelineFactoryBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "pipelineFactoryBase.h"
#include "Gfx/Resource/resource.h"
#include "Gfx/Resource/resourcePools.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
pipelineFactoryBase::~pipelineFactoryBase() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
pipelineFactoryBase::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->pointers = ptrs;
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
pipelineFactoryBase::Discard() {
    o_assert_dbg(this->isValid);
    this->pointers = gfxPointers();
    this->isValid = false;
}

//------------------------------------------------------------------------------
ResourceState::Code
pipelineFactoryBase::SetupResource(pipeline& pip) {
    o_assert_dbg(this->isValid);
    pip.shd = this->pointers.shaderPool->Lookup(pip.Setup.Shader);
    o_assert_dbg(pip.shd && (ResourceState::Valid == pip.shd->State));
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
pipelineFactoryBase::DestroyResource(pipeline& pip) {
    pip.Clear();
}

} // namespace _priv
} // namespace Oryol
