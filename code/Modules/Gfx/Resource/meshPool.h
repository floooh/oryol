#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::meshPool
    @ingroup _priv
    @brief resource pool specialization for meshes
*/
#include "Resource/Core/ResourcePool.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/Setup/MeshSetup.h"

namespace Oryol {
namespace _priv {
class meshPool : public ResourcePool<mesh, MeshSetup> { };
} // namespace _priv
} // namepspace Oryol