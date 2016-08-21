#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkPipelineFactory
    @ingroup _priv
    @brief Vulkan implementation of pipelineFactory
*/
#include "Gfx/Resource/pipelineFactoryBase.h"
#include "vlk_impl.h"

namespace Oryol {
namespace _priv {

class vlkPipelineFactory : public pipelineFactoryBase {
public:
    /// setup the factory
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// setup pipeline resource
    ResourceState::Code SetupResource(pipeline& pip);
    /// destroy pipeline resource
    void DestroyResource(pipeline& pip);

    VkPipelineCache pipelineCache = nullptr;
};

} // namespace _priv
} // namespace Oryol
