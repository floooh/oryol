#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::shaderPool
    @brief resource pool specialization for shaders
*/
#include "Resource/Pool.h"
#include "Render/Setup/ShaderSetup.h"
#include "Render/Core/shaderFactory.h"
#include "Render/Core/shader.h"

namespace Oryol {
namespace Render {
    
class shaderPool : public Resource::Pool<shader, ShaderSetup, shaderFactory> { };
    
} // namespace Render
} // namespace Oryol
