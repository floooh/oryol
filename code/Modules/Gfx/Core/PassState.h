#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::PassState
    @ingroup Gfx
    @brief describes override parameters for Gfx::BeginPass()
*/
#include "Core/Types.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/Core/GfxConfig.h"
#include "glm/vec4.hpp"
#include <initializer_list>

namespace Oryol {

class PassState {
public:
    /// default constructor
    PassState() : Depth(1.0f), Stencil(0) { };
    /// construct with clear color (all attachment to same color), optional depth/stencil
    PassState(const glm::vec4& color, float depth=1.0f, uint8_t stencil=0) {
        for (auto& c : this->Color) {
            c = color;
        }
        this->Depth = depth;
        this->Stencil = stencil;
    }
    /// construct with MRT clear colors
    PassState(std::initializer_list<glm::vec4> colors, float depth=1.0f, uint8_t stencil=0) {
        int i = 0;
        for (const auto& c : colors) {
            this->Color[i++] = c;
        }
        this->Depth = depth;
        this->Stencil = stencil;
    }

    /// override clear colors
    StaticArray<glm::vec4, GfxConfig::MaxNumColorAttachments> Color;
    /// override clear depth value
    float Depth;
    /// override clear-stencil value
    uint8_t Stencil;
};

} // namespace Oryol
