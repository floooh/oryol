#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::renderer
    @ingroup _priv
    @brief main rendering API wrapper
 */
#if ORYOL_OPENGL
#include "Gfx/gl/glRenderer.h"
namespace Oryol {
namespace _priv {
class renderer : public glRenderer { };
} }
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11Renderer.h"
namespace Oryol {
namespace _priv {
class renderer : public d3d11Renderer { };
} }
#elif ORYOL_METAL
#include "Gfx/mtl/mtlRenderer.h"
namespace Oryol {
namespace _priv {
class renderer : public mtlRenderer { };
} }
#else
#error "Target platform not yet supported!"
#endif
