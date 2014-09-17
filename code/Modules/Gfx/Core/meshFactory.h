#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::meshFactory
    @ingroup _priv
    @brief private: resource factory for Mesh objects
    @todo describe meshFactory
*/
#include "Gfx/base/meshLoaderBase.h"
#if ORYOL_OPENGL
#include "Gfx/gl/glMeshFactory.h"
namespace Oryol {
namespace _priv {
class meshFactory : public glMeshFactory {
public:
    /// attach a loader
    void AttachLoader(const Ptr<meshLoaderBase>& loader);
};
#else
#error "Platform not yet supported!"
#endif

//------------------------------------------------------------------------------
inline void
meshFactory::AttachLoader(const Ptr<meshLoaderBase>& loader) {
    o_assert(loader.isValid());
    o_assert(InvalidIndex == this->loaders.FindIndexLinear(loader));
    loader->onAttachToFactory(this);
    this->loaders.Add(loader);
}

} // namespace _priv
} // namespace Oryol

 