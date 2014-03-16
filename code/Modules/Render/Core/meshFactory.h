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
class meshFactory : public glMeshFactory { };
} // namespace Render
} // namespace Oryol

#else
#error "Platform not yet supported!"
#endif
 
 