#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::blendStateFactory
    @brief factory class for blendState resource
*/
#include "Render/base/simpleFactory.h"
#include "Render/Core/blendState.h"

namespace Oryol {
namespace Render {
class blendStateFactory : public simpleFactory<blendState, ResourceType::BlendState> { };
} // namespace Render
} // namespace Oryol
