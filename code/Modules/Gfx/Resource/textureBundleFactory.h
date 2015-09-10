#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureBundleFactory
    @ingroup _priv
    @brief factory class for texture bundle objects
    
    Texture bundles group a collection of textures that need to be set
    on a shader. A texture bundle object will only become valid after
    all its textures have become valid (see gfxResourceContainer for that).
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glTextureBundleFactory.h"
namespace Oryol {
namespace _priv {
class textureBundleFactory : public glTextureBundleFactory { };
} }
#elif ORYOL_METAL
#include "Gfx/mtl/mtlTextureBundleFactory.h"
namespace Oryol {
namespace _priv {
class textureBundleFactory : public mtlTextureBundleFactory { };
} }
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11TextureBundleFactory.h"
namespace Oryol {
namespace _priv {
class textureBundleFactory : public d3d11TextureBundleFactory { };
} }
#else
#error "Target platform not supported!"
#endif
