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
class meshFactory : public glMeshFactory { };
#else
#error "Platform not yet supported!"
#endif

} // namespace _priv
} // namespace Oryol

 