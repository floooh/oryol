#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::drawState
    @ingroup _priv
    @brief bundles pre-compiled state for drawing operations
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glDrawState.h"
namespace Oryol {
namespace _priv {
class drawState : public glDrawState { };
} }
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11DrawState.h"
namespace Oryol {
namespace _priv {
class drawState : public d3d11DrawState { };
} }
#elif ORYOL_METAL
#include "Gfx/Resource/drawStateBase.h"
namespace Oryol {
namespace _priv {
class drawState : public drawStateBase { };
} }
#else
#error "Target platform not yet supported!"
#endif
