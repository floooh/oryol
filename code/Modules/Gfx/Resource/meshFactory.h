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
#elif ORYOL_D3D12
#include "Gfx/d3d12/d3d12MeshFactory.h"
namespace Oryol {
namespace _priv {
class meshFactory : public d3d12MeshFactory { };
} }
#elif ORYOL_METAL
#include "Gfx/mtl/mtlMeshFactory.h"
namespace Oryol {
namespace _priv {
class meshFactory : public mtlMeshFactory { };
} }
#elif ORYOL_VULKAN
#include "Gfx/vlk/vlkMeshFactory.h"
namespace Oryol {
namespace _priv {
class meshFactory : public vlkMeshFactory { };
} }
#else
#error "Platform not yet supported!"
#endif
 