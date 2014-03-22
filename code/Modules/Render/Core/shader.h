#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::shader
    @brief simple shader (vertex- or fragment shader)

    A shader object holds a compiled vertex- or fragment-shader. Alone it
    is useless, but it is the basis for linked shader programs.
*/
#if ORYOL_OPENGL
#include "Render/gl/glShader.h"
namespace Oryol {
namespace Render {
class shader : public glShader { };
} // namespace Render
} // namespace Oryol
#else
#error "Target platform not yet supported!"
#endif
