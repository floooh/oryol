#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glPipelineFactory
    @ingroup _priv
    @brief GL implementation of pipelineFactory
*/
#include "Gfx/Resource/pipelineFactoryBase.h"
#include "Gfx/gl/gl_decl.h"
#include "Gfx/Core/GfxTypes.h"

namespace Oryol {
namespace _priv {

class glPipelineFactory : public pipelineFactoryBase {
public:
    /// setup pipeline resource
    ResourceState::Code SetupResource(pipeline& pip);
    /// destroy the pipeline
    void DestroyResource(pipeline& pip);

private:
    /// setup the GL vertex attribute declaration
    void glSetupVertexAttrs(pipeline& pip);
};

} // namespace _priv
} // namespace Oryol
