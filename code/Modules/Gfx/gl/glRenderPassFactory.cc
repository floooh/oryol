//------------------------------------------------------------------------------
//  glRenderPassFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glRenderPassFactory.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
ResourceState::Code
glRenderPassFactory::SetupResource(renderPass& rp) {
    o_assert_dbg(this->isValid);

    renderPassFactoryBase::SetupResource(rp);
    // FIXME
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glRenderPassFactory::DestroyResource(renderPass& rp) {
    o_assert_dbg(this->isValid);

    // FIXME

    renderPassFactoryBase::DestroyResource(rp);
}

} // namespace _priv
} // namespace Oryol
