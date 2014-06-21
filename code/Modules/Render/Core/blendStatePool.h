#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::blendStatePool.h
    @brief resource pool class for blendState resources
*/
#include "Resource/Pool.h"
#include "Render/Setup/BlendStateSetup.h"
#include "Render/Core/blendStateFactory.h"
#include "Render/Core/blendState.h"

namespace Oryol {
namespace Render {
class blendStatePool : public Resource::Pool<blendState, BlendStateSetup, blendStateFactory> { };
} // namespace Render
} // namespace Oryol
