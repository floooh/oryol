#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::meshPool
    @ingroup _priv
    @brief resource pool specialization for meshes
*/
#include "Resource/Pool.h"
#include "Render/Core/mesh.h"
#include "Render/Core/meshFactory.h"
#include "Render/Setup/MeshSetup.h"

namespace Oryol {
namespace _priv {
class meshPool : public Resource::Pool<mesh, MeshSetup, meshFactory> { };
} // namespace _priv
} // namepspace Oryol