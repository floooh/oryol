#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12PipelineFactory
    @ingroup _priv
    @brief D3D12 implementation of pipelineFactory
*/
#include "Gfx/Resource/pipelineFactoryBase.h"

namespace Oryol {
namespace _priv {

class d3d12PipelineFactory : public pipelineFactoryBase {
public:
    /// setup pipeline resource
    ResourceState::Code SetupResource(pipeline& pip);
    /// destroy the pipeline
    void DestroyResource(pipeline& pip);

private:
    /// create pipeline-state-object and set into drawState
    void createPSO(pipeline& pip);
};

} // namespace _priv
} // namespace Oryol