//------------------------------------------------------------------------------
//  d3d12ResourceContainer.cc
//------------------------------------------------------------------------------
#include "d3d12ResourceContainer.h"
#include "Gfx/Core/renderer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
d3d12ResourceContainer::setup(const GfxSetup& setup, const gfxPointers& ptrs) {
    gfxResourceContainerBase::setup(setup, ptrs);
    this->resourceAllocator.Setup(ptrs.renderer->d3d12Device);
}

//------------------------------------------------------------------------------
void
d3d12ResourceContainer::discard() {
    this->resourceAllocator.Discard();
    gfxResourceContainerBase::discard();
}

} // namespace _priv
} // namespace Oryol
