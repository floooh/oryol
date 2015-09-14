#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::shader
    @brief Oryol shader resource class

    A shader resource holds several related shader programs, one of
    which is selected by a bit mask. Selection mask bits are usually
    associated with a specific shader feature, e.g. "skinning", 
    "normal-depth rendering", "shadow-map rendering", etc... The program
    bundle also maps shader variables to common slot indices across
    all contained programs.
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glShader.h"
namespace Oryol {
namespace _priv {
class shader : public glShader { };
} }
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11Shader.h"
namespace Oryol {
namespace _priv {
class shader : public d3d11Shader { };
} }
#elif ORYOL_D3D12
#include "Gfx/d3d12/d3d12Shader.h"
namespace Oryol {
namespace _priv {
class shader : public d3d12Shader { };
} }
#elif ORYOL_METAL
#include "Gfx/mtl/mtlShader.h"
namespace Oryol {
namespace _priv {
class shader : public mtlShader { };
} }
#else
#error "Target platform not yet supported!"
#endif
