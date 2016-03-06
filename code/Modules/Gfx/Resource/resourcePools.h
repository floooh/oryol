#pragma once
//------------------------------------------------------------------------------
/**
    @file Gfx/Resource/resourcePools.h
    @ingroup _priv
    @brief Gfx module resource pool classes
*/
#include "Resource/Core/ResourcePool.h"
#include "Gfx/Setup/PipelineSetup.h"
#include "Gfx/Resource/pipeline.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/Setup/MeshSetup.h"
#include "Gfx/Setup/ShaderSetup.h"
#include "Gfx/Resource/shader.h"
#include "Gfx/Resource/texture.h"
#include "Gfx/Setup/TextureSetup.h"

namespace Oryol {
namespace _priv {

class pipelinePool : public ResourcePool<pipeline, PipelineSetup> { };
class meshPool : public ResourcePool<mesh, MeshSetup> { };
class shaderPool : public ResourcePool<shader, ShaderSetup> { };
class texturePool : public ResourcePool<texture, TextureSetup> { };

} // namespace _priv
} // namespace Oryol
