#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::gfxPointers
    @ingroup _privv
    @brief pointers to the internal Gfx module objects
    
    This is used to internally communicate pointers to Gfx 'singletons', instead
    of the typical singleton macros (which would expose the pointers to the outside world).
*/
#include "Core/Types.h"

namespace Oryol {

namespace _priv {

class displayMgr;

struct gfxPointers {
    class displayMgr* displayMgr = nullptr;
};

} // namespace _priv
} // namespace Oryol
