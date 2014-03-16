#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::StateWrapper
    @brief platform-agnostic wrapper for render state managment
    @todo describe StateWrapper
*/
#if ORYOL_OPENGL
#include "Render/gl/glStateWrapper.h"
namespace Oryol {
namespace Render {
class stateWrapper: public glStateWrapper { };
} // namespace Render
} // namespace Oryol
#else
#error "Platform not yet supported!"
#endif