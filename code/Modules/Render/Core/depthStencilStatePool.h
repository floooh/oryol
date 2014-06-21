#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::depthStencilStatePool
    @brief resource pool specialization for DepthStencilState objects
 */
#include "Resource/Pool.h"
#include "Render/Setup/DepthStencilStateSetup.h"
#include "Render/Core/depthStencilStateFactory.h"
#include "Render/Core/depthStencilState.h"

namespace Oryol {
namespace Render {
class depthStencilStatePool : public Resource::Pool<depthStencilState, DepthStencilStateSetup, depthStencilStateFactory> { };
} // namespace Render
} // namespace Oryol
