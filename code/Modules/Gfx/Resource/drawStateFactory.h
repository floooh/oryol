#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::drawStateFactory
    @ingroup _priv
    @brief factory class for drawState resource objects
*/
#if ORYOL_OPENGL
#include "Gfx/Resource/drawStateFactoryBase.h"
namespace Oryol {
namespace _priv {
class drawStateFactory : public drawStateFactoryBase { };
} }
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11drawStateFactory.h"
namespace Oryol {
namespace _priv {
class drawStateFactory : public d3d11DrawStateFactory { };
} }
#else
#error "Target platform not yet supported!"
#endif
