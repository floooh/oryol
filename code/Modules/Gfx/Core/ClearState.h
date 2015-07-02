#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ClearState
    @ingroup Gfx
    @brief describes render-target clear action and clear values
    
    FIXME: needs to be extended for multiple-render targets
*/
#include "Core/Types.h"
#include "glm/vec4.hpp"

namespace Oryol {

class ClearState {
public:
    enum Bits {
        ClearNone = 0,
        ClearColor = (1<<0),
        ClearDepth = (1<<1),
        ClearStencil = (1<<2),

        ClearDepthStencil = ClearDepth | ClearStencil,
        ClearAll = ClearColor | ClearDepth | ClearStencil
    };
    /// the clear color (default: black)
    glm::vec4 Color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    /// the clear depth value (default: 1.0)
    float32 Depth = 1.0f;
    /// the clear stencil value (default: 0)
    uint8 Stencil = 0;
    /// the clear action(s) (default: ClearNone)
    uint8 Actions = ClearAll;
};

} // namespace Oryol
