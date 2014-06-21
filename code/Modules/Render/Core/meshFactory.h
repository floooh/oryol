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
    void AttachLoader(const Core::Ptr<meshLoaderBase>& loader);
};
#else
#error "Platform not yet supported!"
#endif

//------------------------------------------------------------------------------
inline void
meshFactory::AttachLoader(const Core::Ptr<meshLoaderBase>& loader) {
    o_assert(InvalidIndex == this->loaders.FindIndexLinear(loader));
    loader->onAttachToFactory(this);
    this->loaders.AddBack(loader);
}

} // namespace Render
} // namespace Oryol

 