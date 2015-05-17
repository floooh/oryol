#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::shader
    @ingroup _priv
    @brief simple shader (vertex- or fragment shader)

    A shader object holds a compiled vertex- or fragment-shader. Alone it
    is useless, but it is the basis for linked shader programs.
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glShader.h"
namespace Oryol {
namespace _priv {
class shader : public glShader { };
} }
#elif ORYOL_D3D11
#include "Gfx/Resource/shaderBase.h"
namespace Oryol {
namespace _priv {
class shader : public shaderBase { };
} }
#else
#error "Target platform not yet supported!"
#endif
