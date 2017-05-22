#pragma once
//------------------------------------------------------------------------------
/**
    @file Gfx/Resource/resourcePools.h
    @ingroup _priv
    @brief Gfx module resource pool classes
*/
#include "Resource/ResourcePool.h"
#include "Gfx/private/resource.h"

namespace Oryol {
namespace _priv {

class pipelinePool : public ResourcePool<pipeline, PipelineSetup> { };
class meshPool : public ResourcePool<mesh, MeshSetup> { };
class shaderPool : public ResourcePool<shader, ShaderSetup> { };
class texturePool : public ResourcePool<texture, TextureSetup> { };
class renderPassPool : public ResourcePool<renderPass, PassSetup> { };

} // namespace _priv
} // namespace Oryol
