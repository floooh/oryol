#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::depthStencilState
    @brief resource class to encapsulate a depth-stencil stateblock
*/
#include "Resource/resourceBase.h"
#include "Render/Setup/DepthStencilStateSetup.h"

namespace Oryol {
namespace Render {
    
class depthStencilState : public Resource::resourceBase<DepthStencilStateSetup> {
public:
    /// clear the object
    void clear();
};
    
} // namespace Render
} // namespace Oryol