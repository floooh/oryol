#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12ResourceContainer
    @ingroup _priv
    @brief Gfx resource container with D3D12 extensions
*/
#include "Gfx/Resource/gfxResourceContainerBase.h"
#include "Gfx/d3d12/d3d12ResourceAllocator.h"

namespace Oryol {
namespace _priv {

class d3d12ResourceContainer : public gfxResourceContainerBase {
public:
    /// setup the resource container
    void setup(const GfxSetup& setup, const gfxPointers& ptrs);
    /// discard the resource manager
    void discard();

private:
    d3d12ResourceAllocator resourceAllocator;
};

} // namespace _priv
} // namespace Oryol