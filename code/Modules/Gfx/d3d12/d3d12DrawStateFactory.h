#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12DrawStateFactory
    @ingroup _priv
    @brief D3D12 implementation of drawStateFactory
*/
#include "Gfx/Resource/drawStateFactoryBase.h"
#include "Core/Containers/Map.h"
#include "glm/vec4.hpp"
#include "glm/gtc/epsilon.hpp"

namespace Oryol {
namespace _priv {

class d3d12DrawStateFactory : public drawStateFactoryBase {
public:
    /// setup drawState resource
    ResourceState::Code SetupResource(drawState& ds);
    /// destroy the drawState
    void DestroyResource(drawState& ds);

private:
    /// create pipeline-state-object and set into drawState
    void createPSO(drawState& ds);
};

} // namespace _priv
} // namespace Oryol