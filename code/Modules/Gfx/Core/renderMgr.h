#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::renderMgr
    @ingroup _priv
    @brief frontend-wrapper for internal render manager
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glRenderMgr.h"
namespace Oryol {
namespace _priv {
class renderMgr : public glRenderMgr { };
} }
#else
#error "Target platform not yet supported!"
#endif
