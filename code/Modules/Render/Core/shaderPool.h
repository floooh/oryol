#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::shaderPool
    @ingroup _priv
    @brief resource pool specialization for shaders
*/
#include "Resource/ResourcePool.h"
#include "Render/Setup/ShaderSetup.h"
#include "Render/Core/shaderFactory.h"
#include "Render/Core/shader.h"

namespace Oryol {
namespace _priv {
class shaderPool : public ResourcePool<shader, ShaderSetup, shaderFactory> { };
} // namespace _priv
} // namespace Oryol
