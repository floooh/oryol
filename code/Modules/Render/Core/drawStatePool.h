#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::drawStatePool
    @ingroup _priv
    @brief resource pool for drawState resources
*/
#include "Resource/Pool.h"
#include "Render/Setup/DrawStateSetup.h"
#include "Render/Core/drawStateFactory.h"
#include "Render/Core/drawState.h"

namespace Oryol {
namespace _priv {
class drawStatePool : public Resource::Pool<drawState, DrawStateSetup, drawStateFactory> { };
} // namespace _priv
} // namespace Oryol