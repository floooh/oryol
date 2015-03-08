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
#else
#error "Platform not supported yet!"
#endif

} // namespace _priv
} // namespace Oryol
