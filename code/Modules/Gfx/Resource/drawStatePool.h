#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::drawStatePool
    @ingroup _priv
    @brief resource pool for drawState resources
*/
#include "Resource/Core/ResourcePool.h"
#include "Gfx/Setup/DrawStateSetup.h"
#include "Gfx/Resource/drawState.h"

namespace Oryol {
namespace _priv {
class drawStatePool : public ResourcePool<drawState, DrawStateSetup> { };
} // namespace _priv
} // namespace Oryol