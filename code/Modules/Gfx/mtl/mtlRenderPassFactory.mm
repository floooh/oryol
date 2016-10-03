//------------------------------------------------------------------------------
//  mtlRenderPassFactory.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlRenderPassFactory.h"
#include "mtl_impl.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
ResourceState::Code
mtlRenderPassFactory::SetupResource(renderPass& rp) {
    o_assert_dbg(this->isValid);

    renderPassFactoryBase::SetupResource(rp);
    // FIXME
    return ResourceState::Failed;
}

//------------------------------------------------------------------------------
void
mtlRenderPassFactory::DestroyResource(renderPass& rp) {
    o_assert_dbg(this->isValid);

    // FIXME

    renderPassFactoryBase::DestroyResource(rp);
}

} // namespace _priv
} // namespace Oryol
