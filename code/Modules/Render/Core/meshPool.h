#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::meshPool
    @brief resource pool specialization for meshes
*/
#include "Resource/Pool.h"
#include "Render/Core/mesh.h"
#include "Render/Core/meshFactory.h"
#include "Render/Setup/MeshSetup.h"

namespace Oryol {
namespace Render {

class meshPool : public Resource::Pool<mesh, MeshSetup, meshFactory> { };

} // namespace Render
} // namepspace Oryol