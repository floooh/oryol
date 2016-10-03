#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlPipelineFactory
    @ingroup _priv
    @brief Metal implementation of pipelineFactory
*/
#include "Gfx/Resource/pipelineFactoryBase.h"

namespace Oryol {
namespace _priv {

class mtlPipelineFactory : public pipelineFactoryBase {
public:
    /// setup a new pipeline resource
    ResourceState::Code SetupResource(pipeline& pip);
    /// destroy pipeline resource
    void DestroyResource(pipeline& pip);

private:
    /// create a MTLRenderPipelineState object
    void createRPS(pipeline& pip);
    /// create MTLDepthStencilState object
    void createDSS(pipeline& pip);
};

} // namespace _priv
} // namespace Oryol
