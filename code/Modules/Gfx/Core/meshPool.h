#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::meshPool
    @ingroup _priv
    @brief resource pool specialization for meshes
*/
#include "Resource/ResourcePool.h"
#include "Gfx/Core/mesh.h"
#include "Gfx/Core/meshFactory.h"
#include "Gfx/Setup/MeshSetup.h"

namespace Oryol {
namespace _priv {
class meshPool : public ResourcePool<mesh, MeshSetup, meshFactory> { };
} // namespace _priv
} // namepspace Oryol