#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::shaderFactory
    @ingroup _priv
    @brief resource factory for shader objects
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glShaderFactory.h"
namespace Oryol {
namespace _priv {
class shaderFactory : public glShaderFactory { };
} }
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11ShaderFactory.h"
namespace Oryol {
namespace _priv {
class shaderFactory : public d3d11ShaderFactory { };
} }
#elif ORYOL_D3D12
#include "Gfx/d3d12/d3d12ShaderFactory.h"
namespace Oryol {
namespace _priv {
class shaderFactory : public d3d12ShaderFactory { };
} }
#elif ORYOL_METAL
#include "Gfx/mtl/mtlShaderFactory.h"
namespace Oryol {
namespace _priv {
class shaderFactory : public mtlShaderFactory { };
} }
#elif ORYOL_VULKAN
#include "Gfx/vlk/vlkShaderFactory.h"
namespace Oryol {
namespace _priv {
class shaderFactory : public vlkShaderFactory { };
} }
#else
#error "Platform not yet supported!"
#endif
