#pragma once
//------------------------------------------------------------------------------
/**
    @file Gfx/Resource/resourcePools.h
    @ingroup _priv
    @brief Gfx module resource pool classes
*/
#include "Resource/Core/ResourcePool.h"
#include "Gfx/Setup/DrawStateSetup.h"
#include "Gfx/Resource/drawState.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/Setup/MeshSetup.h"
#include "Gfx/Setup/ShaderSetup.h"
#include "Gfx/Resource/shader.h"
#include "Gfx/Resource/texture.h"
#include "Gfx/Setup/TextureSetup.h"
#include "Gfx/Resource/textureBundle.h"
#include "Gfx/Setup/TextureBundleSetup.h"

namespace Oryol {
namespace _priv {

class drawStatePool : public ResourcePool<drawState, DrawStateSetup> { };
class meshPool : public ResourcePool<mesh, MeshSetup> { };
class shaderPool : public ResourcePool<shader, ShaderSetup> { };
class texturePool : public ResourcePool<texture, TextureSetup> { };
class textureBundlePool : public ResourcePool<textureBundle, TextureBundleSetup> { };

} // namespace _priv
} // namespace Oryol
