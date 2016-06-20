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
    /// create clear-state which doesn't clear
    static ClearState ClearNone();
    /// create clear-state which clears all targets
    static ClearState ClearAll(const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), float depth=1.0f, uint8_t stencil=0);
    /// create clear state which only clears color
    static ClearState ClearColor(const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    /// create clear state which only clears depth-stencil
    static ClearState ClearDepthStencil(float depth=1.0f, uint8_t stencil=0);

    enum ClearActionBits {
        None = 0,
        ColorBit = (1<<0),
        DepthBit = (1<<1),
        StencilBit = (1<<2),

        DepthStencilBits = DepthBit | StencilBit,
        AllBits = ColorBit | DepthBit
    };
    /// the clear color (default: black)
    glm::vec4 Color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    /// the clear depth value (default: 1.0)
    float Depth = 1.0f;
    /// the clear stencil value (default: 0)
    uint8_t Stencil = 0;
    /// the clear action(s) (default: ClearAll)
    uint8_t Actions = AllBits;
};

} // namespace Oryol
