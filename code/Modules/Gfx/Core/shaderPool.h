#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::shaderPool
    @ingroup _priv
    @brief resource pool specialization for shaders
*/
#include "Resource/ResourcePool.h"
#include "Gfx/Setup/ShaderSetup.h"
#include "Gfx/Core/shaderFactory.h"
#include "Gfx/Core/shader.h"

namespace Oryol {
namespace _priv {
class shaderPool : public ResourcePool<shader, ShaderSetup, shaderFactory> { };
} // namespace _priv
} // namespace Oryol
