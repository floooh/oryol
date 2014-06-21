#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::depthStencilStateFactory
    @brief private: resource factory for depthStencilState objects
*/
#include "Render/base/simpleFactory.h"
#include "Render/Core/depthStencilState.h"

namespace Oryol {
namespace Render {
class depthStencilStateFactory : public simpleFactory<depthStencilState, ResourceType::DepthStencilState> { };
} // namespace Render
} // namespace Oryol
