#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::renderer
    @ingroup _priv
    @brief main rendering API wrapper
 */
#if ORYOL_OPENGL
#include "Gfx/gl/glRenderer.h"
namespace Oryol {
namespace _priv {
class renderer : public glRenderer { };
} }
#else
#error "Target platform not yet supported!"
#endif
