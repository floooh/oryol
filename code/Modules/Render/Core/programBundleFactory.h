#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::programBundleFactory
    @brief private: resource factory for program bundle objects
*/
#if ORYOL_OPENGL
#include "Render/gl/glProgramBundleFactory.h"
namespace Oryol {
namespace Render {
class programBundleFactory : public glProgramBundleFactory { };
} // namespace Render
} // namespace Oryol
#else
#error "Platform not yet supported!"
#endif
