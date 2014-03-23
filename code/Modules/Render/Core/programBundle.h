#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::programBundle
    @brief a shader program bundle

    A programBundle resource holds several related shader programs, one of
    which is selected by a bit mask. Selection mask bits are usually
    associated with a specific shader feature, e.g. "skinning", 
    "normal-depth rendering", "shadow-map rendering", etc... The program
    bundle also maps shader variables to common slot indices across
    all contained programs.
*/
#if ORYOL_OPENGL
#include "Render/gl/glProgramBundle.h"
namespace Oryol {
namespace Render {
class programBundle : public glProgramBundle { };
} // namespace Render
} // namespace Oryol
#else
#error "Target platform not yet supported!"
#endif
