#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::textureFactory
    @brief private: resource factory to texture objects
    @todo describe texture factory
*/
#include "Render/base/textureLoaderBase.h"
#if ORYOL_OPENGL
#include "Render/gl/glTextureFactory.h"
namespace Oryol {
namespace Render {    
class textureFactory : public glTextureFactory {
public:
    /// attach a loader
    void AttachLoader(const Core::Ptr<textureLoaderBase>& loader);
};
#else
#error "Platform not supported yet!"
#endif

//------------------------------------------------------------------------------
inline void
textureFactory::AttachLoader(const Core::Ptr<textureLoaderBase>& loader) {
    o_assert(InvalidIndex == this->loaders.FindIndexLinear(loader));
    loader->onAttachToFactory(this);
    this->loaders.AddBack(loader);
}

} // namespace Render
} // namespace Oryol
