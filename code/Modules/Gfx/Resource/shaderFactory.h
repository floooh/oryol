#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::shaderFactory
    @ingroup _priv
    @brief private: resource factory for shader objects
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glShaderFactory.h"
namespace Oryol {
namespace _priv {
class shaderFactory : public glShaderFactory { };
} // namespace _priv
} // namespace Oryol
#else
#error "Platform not yet supported!"
#endif
