#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureBlockFactory
    @ingroup _priv
    @brief factory class for texture block objects
    
    Texture blovkc group a collection of textures that need to be set
    on a shader. A texture block object will only become valid after
    all its textures have become valid (see gfxResourceContainer for that).
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glTextureBlockFactory.h"
namespace Oryol {
namespace _priv {
class textureBlockFactory : public glTextureBlockFactory { };
} }
#elif ORYOL_METAL
#include "Gfx/mtl/mtlTextureBlockFactory.h"
namespace Oryol {
namespace _priv {
class textureBlockFactory : public mtlTextureBlockFactory { };
} }
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11TextureBlockFactory.h"
namespace Oryol {
namespace _priv {
class textureBlockFactory : public d3d11TextureBlockFactory { };
} }
#elif ORYOL_D3D12
#include "Gfx/d3d12/d3d12TextureBlockFactory.h"
namespace Oryol {
namespace _priv {
class textureBlockFactory : public d3d12TextureBlockFactory { };
} }
#else
#error "Target platform not supported!"
#endif
