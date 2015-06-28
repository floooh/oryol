//------------------------------------------------------------------------------
//  mtlShaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlShaderFactory.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlShaderFactory::mtlShaderFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
mtlShaderFactory::~mtlShaderFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
mtlShaderFactory::Setup(renderer* /*rendr*/) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
mtlShaderFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
mtlShaderFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlShaderFactory::SetupResource(shader& shd) {
    o_assert_dbg(this->isValid);

    o_error("FIXME: mtlShaderFactory::SetupResource()\n");

    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
mtlShaderFactory::DestroyResource(shader& shd) {
    o_assert_dbg(this->isValid);

    o_error("FIXME: mtlShaderFactory::DestroyResource9)\n");

}

} // namespace _priv
} // namespace Oryol
