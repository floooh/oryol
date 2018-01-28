#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::gfxBackend
    @ingroup _priv
    @brief wrapper class for gfx backend implementations
*/
#include "Gfx/private/sokol/sokolGfxBackend.h"

namespace Oryol {
namespace _priv {
class gfxBackend : public sokolGfxBackend { };
} // namespace _priv
} // namespace Oryol
