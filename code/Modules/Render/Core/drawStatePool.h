#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::drawStatePool
    @brief resource pool for drawState resources
*/
#include "Resource/Pool.h"
#include "Render/Setup/DrawStateSetup.h"
#include "Render/Core/drawStateFactory.h"
#include "Render/Core/drawState.h"

namespace Oryol {
namespace Render {
class drawStatePool : public Resource::Pool<drawState, DrawStateSetup, drawStateFactory> { };
} // namespace Render
} // namespace Oryol