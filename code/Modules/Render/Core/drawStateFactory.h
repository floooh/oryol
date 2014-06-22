#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::drawStateFactory
    @brief factory class for drawState resources
*/
#include "Render/base/simpleFactory.h"
#include "Render/Core/drawState.h"

namespace Oryol {
namespace Render {
class drawStateFactory : public simpleFactory<drawState, ResourceType::DrawState> { };
} // namespace Render
} // namespace Oryol