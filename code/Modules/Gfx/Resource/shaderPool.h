#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::shaderPool
    @ingroup _priv
    @brief resource pool specialization for shaders
*/
#include "Resource/Core/ResourcePool.h"
#include "Gfx/Setup/ShaderSetup.h"
#include "Gfx/Resource/shader.h"

namespace Oryol {
namespace _priv {
class shaderPool : public ResourcePool<shader, ShaderSetup> { };
} // namespace _priv
} // namespace Oryol
