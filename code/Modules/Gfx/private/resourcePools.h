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

class pipelinePool : public ResourcePool<pipeline> { };
class meshPool : public ResourcePool<mesh> { };
class shaderPool : public ResourcePool<shader> { };
class texturePool : public ResourcePool<texture> { };
class renderPassPool : public ResourcePool<renderPass> { };

} // namespace _priv
} // namespace Oryol
