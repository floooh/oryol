#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::meshFactory
    @ingroup _priv
    @brief private: resource factory for Mesh objects
    @todo describe meshFactory
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glMeshFactory.h"
namespace Oryol {
namespace _priv {
class meshFactory : public glMeshFactory { };
} }
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11MeshFactory.h"
namespace Oryol {
namespace _priv {
class meshFactory : public d3d11MeshFactory { };
} }
#else
#error "Platform not yet supported!"
#endif
 