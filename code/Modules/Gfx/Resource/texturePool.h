#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::texturePool
    @ingroup _priv
    @brief resource pool specialization for textures
*/
#include "Resource/Core/ResourcePool.h"
#include "Gfx/Resource/texture.h"
#include "Gfx/Resource/textureFactory.h"
#include "Gfx/Setup/TextureSetup.h"

namespace Oryol {
namespace _priv {
    
class texturePool : public ResourcePool<texture, TextureSetup, textureFactory> {
    /// get the resource type this factory produces
    uint16 GetResourceType() const;
    /// attach a resource loader
    void AttachLoader(const Ptr<textureLoaderBase>& loader);
    /// determine whether asynchronous loading has finished
    bool NeedsSetupResource(const texture& tex) const;
    /// destroy the resource
    void DestroyResource(texture& tex);
};

} // namespace _priv
} // namespace Oryol