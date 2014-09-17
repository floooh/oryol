#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureFactory
    @ingroup _priv
    @brief private: resource factory to texture objects
    @todo describe texture factory
*/
#include "Gfx/base/textureLoaderBase.h"
#if ORYOL_OPENGL
#include "Gfx/gl/glTextureFactory.h"
namespace Oryol {
namespace _priv {
class textureFactory : public glTextureFactory {
public:
    /// attach a loader
    void AttachLoader(const Ptr<textureLoaderBase>& loader);
};
#else
#error "Platform not supported yet!"
#endif

//------------------------------------------------------------------------------
inline void
textureFactory::AttachLoader(const Ptr<textureLoaderBase>& loader) {
    o_assert(loader.isValid());
    o_assert(InvalidIndex == this->loaders.FindIndexLinear(loader));
    loader->onAttachToFactory(this);
    this->loaders.Add(loader);
}

} // namespace _priv
} // namespace Oryol
