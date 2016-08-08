#pragma once
//------------------------------------------------------------------------------
/**
    @file Gfx/Resource/resourcePools.h
    @ingroup _priv
    @brief Gfx module resource pool classes
*/
#include "Resource/Core/ResourcePool.h"
#include "Gfx/Resource/resource.h"

namespace Oryol {
namespace _priv {

class pipelinePool : public ResourcePool<pipeline, PipelineSetup> { };
class meshPool : public ResourcePool<mesh, MeshSetup> { };
class shaderPool : public ResourcePool<shader, ShaderSetup> { };
class texturePool : public ResourcePool<texture, TextureSetup> { };

} // namespace _priv
} // namespace Oryol
