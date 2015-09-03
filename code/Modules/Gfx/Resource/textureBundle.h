#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureBundle
    @ingroup Gfx
    @brief groups multiple textures to be bound to a shader
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glTextureBundle.h"
namespace Oryol {
namespace _priv {
class textureBundle : public glTextureBundle { };
} }
#elif ORYOL_D3D11
#include "Gfx/Resource/textureBundleBase.h"
namespace Oryol {
namespace _priv {
class textureBundle : public textureBundleBase { };
} }
#elif ORYOL_D3D12
#include "Gfx/Resource/textureBundleBase.h"
namespace Oryol {
namespace _priv {
class textureBundle : public textureBundleBase { };
} }
#elif ORYOL_METAL
#include "Gfx/Resource/textureBundleBase.h"
namespace Oryol {
namespace _priv {
class textureBundle : public textureBundleBase { };
} }
#else
#error "Target platform not supported!"
#endif
