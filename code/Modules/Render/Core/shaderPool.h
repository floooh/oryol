#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::shaderPool
    @ingroup _priv
    @brief resource pool specialization for shaders
*/
#include "Resource/Pool.h"
#include "Render/Setup/ShaderSetup.h"
#include "Render/Core/shaderFactory.h"
#include "Render/Core/shader.h"

namespace Oryol {
namespace _priv {
class shaderPool : public Resource::Pool<shader, ShaderSetup, shaderFactory> { };
} // namespace _priv
} // namespace Oryol
