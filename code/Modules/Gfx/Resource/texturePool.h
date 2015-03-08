#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::texturePool
    @ingroup _priv
    @brief resource pool specialization for textures
*/
#include "Resource/Core/ResourcePool.h"
#include "Gfx/Resource/texture.h"
#include "Gfx/Setup/TextureSetup.h"

namespace Oryol {
namespace _priv {
    
class texturePool : public ResourcePool<texture, TextureSetup> { };

} // namespace _priv
} // namespace Oryol