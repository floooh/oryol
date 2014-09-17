#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::StateWrapper
    @ingroup _priv
    @brief platform-agnostic wrapper for render state management
    @todo describe StateWrapper
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glStateWrapper.h"
namespace Oryol {
namespace _priv {
class stateWrapper: public glStateWrapper { };
} // namespace _priv
} // namespace Oryol
#else
#error "Platform not yet supported!"
#endif