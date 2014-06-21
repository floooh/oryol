#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::shaderFactory
    @brief private: resource factory for shader objects
*/
#if ORYOL_OPENGL
#include "Render/gl/glShaderFactory.h"
namespace Oryol {
namespace Render {
class shaderFactory : public glShaderFactory { };
} // namespace Render
} // namespace Oryol
#else
#error "Platform not yet supported!"
#endif
