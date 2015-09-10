#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureBlock
    @ingroup Gfx
    @brief groups multiple textures to be bound to a shader
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glTextureBlock.h"
namespace Oryol {
namespace _priv {
class textureBlock : public glTextureBlock { };
} }
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11TextureBlock.h"
namespace Oryol {
namespace _priv {
class textureBlock : public d3d11TextureBlock { };
} }
#elif ORYOL_D3D12
#include "Gfx/d3d12/d3d12TextureBlock.h"
namespace Oryol {
namespace _priv {
class textureBlock : public d3d12TextureBlock { };
} }
#elif ORYOL_METAL
#include "Gfx/mtl/mtlTextureBlock.h"
namespace Oryol {
namespace _priv {
class textureBlock : public mtlTextureBlock { };
} }
#else
#error "Target platform not supported!"
#endif
