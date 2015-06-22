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

class renderer;
class displayMgr;
class meshPool;
class shaderPool;
class programBundlePool;
class texturePool;
class drawStatePool;

struct gfxPointers {
public:
    class renderer* renderer = nullptr;
    class displayMgr* displayMgr = nullptr;
    class meshPool* meshPool = nullptr;
    class shaderPool* shaderPool = nullptr;
    class programBundlePool* programBundlePool = nullptr;
    class texturePool* texturePool = nullptr;
    class drawStatePool* drawStatePool = nullptr;
};

} // namespace _priv
} // namespace Oryol
