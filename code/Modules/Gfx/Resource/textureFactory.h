#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureFactory
    @ingroup _priv
    @brief private: resource factory to texture objects
    @todo describe texture factory
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glTextureFactory.h"
namespace Oryol {
namespace _priv {
class textureFactory : public glTextureFactory { };
} }
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11TextureFactory.h"
namespace Oryol {
namespace _priv {
class textureFactory : public d3d11TextureFactory { };
}}
#elif ORYOL_METAL
#include "Gfx/mtl/mtlTextureFactory.h"
namespace Oryol {
namespace _priv {
class textureFactory : public mtlTextureFactory { };
}}
#else
#error "Platform not supported yet!"
#endif
