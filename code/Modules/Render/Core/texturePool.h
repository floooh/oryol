#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::texturePool
    @ingroup _priv
    @brief resource pool specialization for textures
*/
#include "Resource/Pool.h"
#include "Render/Core/texture.h"
#include "Render/Core/textureFactory.h"
#include "Render/Setup/TextureSetup.h"

namespace Oryol {
namespace _priv {
    
class texturePool : public Resource::Pool<texture, TextureSetup, textureFactory> {
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