#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::meshFactory
    @brief private: resource factory for Mesh objects
    @todo describe meshFactory
*/
#include "Render/base/meshLoaderBase.h"
#if ORYOL_OPENGL
#include "Render/gl/glMeshFactory.h"
namespace Oryol {
namespace Render {
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

} // namespace Render
} // namespace Oryol

 