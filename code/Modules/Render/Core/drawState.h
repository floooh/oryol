#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::drawState
    @brief bundles pre-compiled state for drawing operations
*/
#include "Resource/resourceBase.h"
#include "Render/Setup/DrawStateSetup.h"

namespace Oryol {
namespace Render {
class drawState : public Resource::resourceBase<DrawStateSetup> { };
} // namespace Render
} // namespace Oryol