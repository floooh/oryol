#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::textureFactory
    @brief private: resource factory to texture objects
    @todo describe texture factory
*/
#if ORYOL_OPENGL
#include "Render/gl/glTextureFactory.h"
namespace Oryol {
namespace Render {
    
class textureFactory : public glTextureFactory {
public:
    /// get the resource type this factory produces
    uint16 GetResourceType() const;
    /// attach a resource loader
    void AttachLoader(const Core::Ptr<textureLoaderBase>& loader);
    /// determine whether asynchronous loading has finished
    bool NeedsSetupResource(const texture& tex) const;
    /// destroy the resource
    void DestroyResource(texture& tex);
};
} // namespace Render
} // namespace Oryol
#else
#error "Platform not supported yet!"
#endif
