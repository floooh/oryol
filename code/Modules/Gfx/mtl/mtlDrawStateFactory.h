#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlDrawStateFactory
    @ingroup _priv
    @brief Metal implementation of drawStateFactory
*/
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/Resource/drawStateFactoryBase.h"
#include "Gfx/mtl/mtl_decl.h"

namespace Oryol {
namespace _priv {

class mtlDrawStateFactory : public drawStateFactoryBase {
public:
    /// setup a new drawState resource
    ResourceState::Code SetupResource(drawState& ds);
    /// destroy drawState resource
    void DestroyResource(drawState& ds);

private:
    /// create a MTLRenderPipelineState object in drawState
    void createRPS(drawState& ds);
    /// release MTLRenderPipelineState object in drawState
    void releaseRPS(drawState& ds);
    /// create MTLDepthStencilState object in drawState
    void createDSS(drawState& ds);
    /// release MTLDepthStencilState object in drawState
    void releaseDSS(drawState& ds);
};

} // namespace _priv
} // namespace Oryol
