#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::renderMgr
    @brief frontend-wrapper for internal render manager
*/
#if ORYOL_OPENGL
#include "Render/gl/glRenderMgr.h"
namespace Oryol {
namespace Render {
class renderMgr : public glRenderMgr { };
} }
#else
#error "Target platform not yet supported!"
#endif
