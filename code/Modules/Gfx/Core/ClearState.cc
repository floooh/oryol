//------------------------------------------------------------------------------
//  ClearState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ClearState.h"

namespace Oryol {

//------------------------------------------------------------------------------
ClearState
ClearState::ClearNone() {
    ClearState state;
    state.Actions = ClearState::None;
    state.Color.Fill(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    return state;
}

//------------------------------------------------------------------------------
ClearState
ClearState::ClearAll(const glm::vec4& color, float depth, uint8_t stencil) {
    ClearState state;
    state.Color[0] = color;
    state.Depth = depth;
    state.Stencil = stencil;
    state.Actions = ClearState::AllBits;
    return state;
}

//------------------------------------------------------------------------------
ClearState
ClearState::ClearColor(const glm::vec4& color) {
    ClearState state;
    state.Color[0] = color;
    state.Actions = ClearState::ColorBit;
    return state;
}

//------------------------------------------------------------------------------
ClearState
ClearState::ClearDepthStencil(float depth, uint8_t stencil) {
    ClearState state;
    state.Depth = depth;
    state.Stencil = stencil;
    state.Actions = ClearState::DepthStencilBits;
    return state;
}

} // namespace Oryol
