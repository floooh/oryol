//------------------------------------------------------------------------------
//  mtlProgramBundleFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlProgramBundleFactory.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlProgramBundleFactory::mtlProgramBundleFactory() :
renderer(0),
shdPool(0),
shdFactory(0),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
mtlProgramBundleFactory::~mtlProgramBundleFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
mtlProgramBundleFactory::Setup(class renderer* rendr, shaderPool* pool, shaderFactory* factory) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(nullptr != rendr);
    o_assert_dbg(nullptr != pool);
    o_assert_dbg(nullptr != factory);
    this->isValid = true;
    this->renderer = rendr;
    this->shdPool = pool;
    this->shdFactory = factory;
}

//------------------------------------------------------------------------------
void
mtlProgramBundleFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->renderer = nullptr;
    this->shdPool = nullptr;
    this->shdFactory = nullptr;
}

//------------------------------------------------------------------------------
bool
mtlProgramBundleFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlProgramBundleFactory::SetupResource(programBundle& progBundle) {
    o_assert_dbg(this->isValid);

    o_error("FIXME: mtlProgramBundleFactory::SetupResource()\n");

    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
mtlProgramBundleFactory::DestroyResource(programBundle& progBundle) {
    o_assert_dbg(this->isValid);

    o_error("FIXME: mtlProgramBundleFactory::DestroyResource()\n");
}

} // namespace _priv
} // namespace Oryol
