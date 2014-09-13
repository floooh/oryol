#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::programBundleFactory
    @ingroup _priv
    @brief private: resource factory for program bundle objects
*/
#if ORYOL_OPENGL
#include "Render/gl/glProgramBundleFactory.h"
namespace Oryol {
namespace _priv {
class programBundleFactory : public glProgramBundleFactory { };
} // namespace _priv
} // namespace Oryol
#else
#error "Platform not yet supported!"
#endif
