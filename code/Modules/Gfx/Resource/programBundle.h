#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::programBundle
    @brief a shader program bundle

    A programBundle resource holds several related shader programs, one of
    which is selected by a bit mask. Selection mask bits are usually
    associated with a specific shader feature, e.g. "skinning", 
    "normal-depth rendering", "shadow-map rendering", etc... The program
    bundle also maps shader variables to common slot indices across
    all contained programs.
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glProgramBundle.h"
namespace Oryol {
namespace _priv {
class programBundle : public glProgramBundle { };
} // namespace _priv
} // namespace Oryol
#elif ORYOL_D3D11
#include "Gfx/Resource/programBundleBase.h"
namespace Oryol {
namespace _priv {
class programBundle : public programBundleBase { };
} }
#else
#error "Target platform not yet supported!"
#endif
