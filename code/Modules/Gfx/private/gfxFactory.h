#pragma once
//------------------------------------------------------------------------------
#if ORYOL_OPENGL
#include "Gfx/private/gl/glFactory.h"
#elif ORYOL_D3D11
#include "Gfx/private/d3d11/d3d11Factory.h"
#elif ORYOL_METAL
#include "Gfx/private/mtl/mtlFactory.h"
#else
#error "Platform not yet supported!"
#endif

namespace Oryol {
namespace _priv {
#if ORYOL_OPENGL
class gfxFactory : public glFactory { };
#elif ORYOL_D3D11
class gfxFactory : public d3d11Factory { };
#elif ORYOL_METAL
class gfxFactory : public mtlFactory { };
#endif

} // namespace _priv
} // namespace Oryol
