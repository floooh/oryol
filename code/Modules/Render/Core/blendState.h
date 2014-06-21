#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::blendState
    @brief resource class to encapsulate blend state
*/
#include "Resource/resourceBase.h"
#include "Render/Setup/BlendStateSetup.h"

namespace Oryol {
namespace Render {
class blendState : public Resource::resourceBase<BlendStateSetup> { };
} // namespace Render
} // namespace Oryol
