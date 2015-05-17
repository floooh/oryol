#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::programBundleFactory
    @ingroup _priv
    @brief private: resource factory for program bundle objects
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glProgramBundleFactory.h"
namespace Oryol {
namespace _priv {
class programBundleFactory : public glProgramBundleFactory { };
} }
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11ProgramBundleFactory.h"
namespace Oryol {
namespace _priv {
class programBundleFactory : public d3d11ProgramBundleFactory { };
} }
#else
#error "Platform not yet supported!"
#endif
