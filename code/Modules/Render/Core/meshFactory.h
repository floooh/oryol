#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::meshFactory
    @brief private: resource factory for Mesh objects
    @todo describe meshFactory
*/
#if ORYOL_OPENGL
#include "Render/gl/glMeshFactory.h"
namespace Oryol {
namespace Render {

class meshFactory : public glMeshFactory {
public:
    /// attach a resource loader
    void AttachLoader(const Core::Ptr<meshLoaderBase>& loader);
    /// determine whether asynchronous loading has finished
    bool NeedsSetupResource(const mesh& mesh) const;
    /// destroy the resource
    void DestroyResource(mesh& mesh);
};

} // namespace Render
} // namespace Oryol
#else
#error "Platform not yet supported!"
#endif
 
 