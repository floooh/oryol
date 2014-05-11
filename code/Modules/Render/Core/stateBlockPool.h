#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::stateBlockPool
    @brief resource pool specialization for StateBlocks
*/
#include "Resource/Pool.h"
#include "Render/Setup/StateBlockSetup.h"
#include "Render/Core/stateBlockFactory.h"
#include "Render/Core/stateBlock.h"

namespace Oryol {
namespace Render {
    
class stateBlockPool : public Resource::Pool<stateBlock, StateBlockSetup, stateBlockFactory> { };
    
} // namespace Render
} // namespace Oryol

